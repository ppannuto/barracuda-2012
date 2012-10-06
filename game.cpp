#include "game.hpp"
#include "strategy.hpp"

#include <iostream>
#include <cassert>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

GameState::GameState(
		const int* board,
		int our_credits,
		int max_opp_credits,
		int turn_number):
	// private
	our_credits(our_credits),
	max_opp_credits(max_opp_credits),
	turn_number(turn_number),
	// public
	board(board)
{
	memset(owned_squares_row_maj, 0, sizeof(owned_squares_row_maj));
	memset(owned_squares_col_maj, 0, sizeof(owned_squares_col_maj));
}

GameState::GameState(
		const GameState& base,
		const GameMove& move,
		int bid
		):
	// private
	our_credits(base.our_credits),
	max_opp_credits(base.max_opp_credits),
	turn_number(base.turn_number+1),
	// public
	board(base.board)
{
	if (move.id > -1) {
		PlayMove(move.id, move.x, move.y);
		if (move.IsMyMove())
			our_credits -= bid;
		else
			max_opp_credits -= bid;
	}
}

void GameState::GetMoves(GameMove output[15])
{
	int put = 0;
	int start = (turn_number % 7) * 7;
	int end = start + 7;

	// Provide the null move.
	output[put].x = output[put].y = 0;
	output[put++].id = -1;

	// Find all other valid moves.
	for (int i = 0; i<49; ++i)
	{
		int y = i / 7;
		int x = i % 7;
		if (board[i] >= start && board[i] < end && !GET_BIT(owned_squares_row_maj[0], x+y*7) && !GET_BIT(owned_squares_row_maj[1], x+y*7))
		{
			output[put].x = x;
			output[put].y = y;
			output[put++].id = Game::idx;
			output[put].x = x;
			output[put].y = y;
			output[put++].id = !Game::idx;
		}
	}

	// The remaining moves are invalid.
	for (int remaining = put; remaining < 15; ++remaining)
	{
		output[remaining].x = -1; // Invalidate this move.
	}
}

void GameState::PlayMove(int idx, int x, int y) {
	SET_BIT(owned_squares_row_maj[idx], x+y*7);
	SET_BIT(owned_squares_col_maj[idx], x*7+y);
}

// debug
const char* GameState::color(int x, int y) {
	if (GET_BIT(owned_squares_row_maj[0], x+7*y))
		return Game::HORZ;
	else if (GET_BIT(owned_squares_row_maj[1], x+7*y))
		return Game::VERT;
	else
		return Game::CLEAR;
}

// debug
const char* GameState::color_T(int x, int y) {
	if (GET_BIT(owned_squares_col_maj[0], x+7*y))
		return Game::HORZ;
	else if (GET_BIT(owned_squares_col_maj[1], x+7*y))
		return Game::VERT;
	else
		return Game::CLEAR;
}

void GameState::PrintGameState() {
	for (int x = 0; x < 7; x++) {
		printf("   ");
		for (int y = 0; y < 7; y++) {
			printf("%s%3d%s", color(x, y), board[x+7*y], Game::CLEAR);
		}
		printf("\n");
	}
	printf("\n");
	for (int x = 0; x < 7; x++) {
		printf("   ");
		for (int y = 0; y < 7; y++) {
			printf("%s%3d%s", color_T(y, x), board[x+7*y], Game::CLEAR);
		}
		printf("\n");
	}
}

int Game::idx;

const char* Game::BLACK  = "\033[0;40m";
const char* Game::RED    = "\033[0;41m";
const char* Game::GREEN  = "\033[0;42m";
const char* Game::YELLOW = "\033[0;43m";
const char* Game::BLUE   = "\033[0;44m";
const char* Game::PURPLE = "\033[0;45m";
const char* Game::TEAL   = "\033[0;46m";
const char* Game::GREY   = "\033[0;47m";

const char *Game::CLEAR = "\033[m";
const char *Game::HORZ  = Game::YELLOW;
const char *Game::VERT  = Game::BLUE;
const char *Game::WIN   = Game::RED;

std::map<int, int> Game::square_to_x;
std::map<int, int> Game::square_to_y;

Game::Game(
		int player_idx,
		int opponent_id,
		int* board,
		int game_id,
		int credits,
		float remaining_time
	  ):
	// private
	opponent_id(opponent_id),
	turn(0),
	board(board),
	game_id(game_id),
	remaining_time(remaining_time),
	game_state(GameState(board, credits, credits, 0))
{
	idx = player_idx;
#ifdef NUM_PROCS
	num_procs = NUM_PROCS;
	std::cout << "Running with fixed number of processors: " << num_procs << std::endl;
#else
	num_procs = sysconf(_SC_NPROCESSORS_ONLN);
	std::cout << "System has " << num_procs << " processors" << std::endl;
#endif

	std::cout << "We are player " << player_idx << " against " << opponent_id << std::endl;

	square_to_x.clear();
	square_to_y.clear();
	for (int x = 0; x < 7; x++) {
		for (int y = 0; y < 7; y++) {
			square_to_x[board[x+y*7]] = x;
			square_to_y[board[x+y*7]] = y;
		}
	}

	PrintGroups();

	strategy = new GordonStrategy(&game_state, idx);
}

Game::~Game() {
	delete strategy;
	delete board;
}

int Game::Bid(int offers_len, int *offers) {
	assert(offers_len > 0);

	int bid;
	strategy->GetMove(offers_len, offers, bid, next_choice);
	return bid;
}

int Game::MakeChoice() {
	return next_choice;
}

void Game::MoveResult(int idx, int choice) {
	if (idx == -1)
		return;

	game_state.PlayMove(idx, square_to_x[choice], square_to_y[choice]);
}

void Game::GameResult(int winner) {
	PrintGame();

	Strategy s = Strategy(&game_state);

	std::cout << "is win for " << idx << ": " << s.is_winning_board_for(idx) << std::endl;
	std::cout << "is win for " << (idx+1)%2 << ": " << s.is_winning_board_for((idx+1)%2) << std::endl;

	if (winner == 1) {
		if (s.is_winning_board_for(idx)) {
			std::cout << "We won and we proved it" << std::endl;
		} else {
			std::cout << "FUCKKKKK. We won but we don't think so" << std::endl;
		}
	} else if (winner == -1) {
		if (s.is_winning_board_for((idx+1)%2)) {
			std::cout << "They won and we proved it" << std::endl;
		} else {
			std::cout << "FUCKKKKK. They won but we don't think so" << std::endl;
		}
	} else {
		if ( (s.is_winning_board_for(idx)) || (s.is_winning_board_for((idx+1)%2)) ) {
			std::cout << "FUCKKKKK. We tied but we don't think so" << std::endl;
		} else {
			std::cout << "We Tied and we agree. yay" << std::endl;
		}
	}

}

const char* Game::GroupColor(int x, int y) {
	int g = board[x+7*y];
	if (0 <= g && g < 7)
		return Game::RED;
	else if (7 <= g && g < 14)
		return Game::GREEN;
	else if (14 <= g && g < 21)
		return Game::YELLOW;
	else if (21 <= g && g < 28)
		return Game::BLUE;
	else if (28 <= g && g < 35)
		return Game::PURPLE;
	else if (35 <= g && g < 42)
		return Game::TEAL;
	else if (42 <= g && g < 49)
		return Game::GREY;
	std::cout << "g " << g << std::endl;
	assert(false);
}

void Game::PrintGroups() {
	printf("----------------------------\n");
	for (int x = 0; x < 7; x++) {
		printf("   ");
		for (int y = 0; y < 7; y++) {
			printf("%s%3d%s", GroupColor(x, y), board[x+7*y], Game::CLEAR);
		}
		printf("\n");
	}
	printf("----------------------------\n");
	printf("\n");
}

void Game::PrintGame() {
	std::cout << "    ";
	if (idx==0)
		std::cout << HORZ << "US (HORZ)" << CLEAR << "  " << VERT << "THEM" << CLEAR << std::endl;
	else
		std::cout << HORZ << "THEM" << CLEAR << "  " << VERT << "US (VERT)" << CLEAR << std::endl;
	std::cout << std::endl;

	game_state.PrintGameState();
}
