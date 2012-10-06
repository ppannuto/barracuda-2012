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
	board(board),
	our_credits(our_credits),
	max_opp_credits(max_opp_credits),
	turn_number(turn_number) {
		memset(owned_squares_row_maj, 0, sizeof(owned_squares_row_maj));
		memset(owned_squares_col_maj, 0, sizeof(owned_squares_col_maj));
}

GameState::GameState(
		const GameState& base,
		const GameMove& move,
		int bid
		):
	board(base.board),
	our_credits(base.our_credits),
	max_opp_credits(base.max_opp_credits),
	turn_number(base.turn_number+1)
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
		for (int y = 0; y < 7; y++) {
			printf("%s%3d%s", color(x, y), board[x+7*y], Game::CLEAR);
		}
		printf("\n");
	}
	printf("\n");
	for (int x = 0; x < 7; x++) {
		for (int y = 0; y < 7; y++) {
			printf("%s%3d%s", color_T(y, x), board[x+7*y], Game::CLEAR);
		}
		printf("\n");
	}
}

int Game::idx;
const char *Game::CLEAR = "\033[m";
const char *Game::HORZ  = "\033[0;43m";
const char *Game::VERT  = "\033[0;44m";
const char *Game::WIN   = "\033[0;41m";

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
}

Game::~Game() {
	delete board;
}

int Game::Bid(int offers_len, int *offers) {
	assert(offers_len > 0);
	next_choice = offers[0];
	return 1;
}

int Game::MakeChoice() {
	return next_choice;
}

void Game::MoveResult(int idx, int choice) {
	if (idx == -1)
		return;

	int x;
	int y;

	// Temp hack
	for (x=0; x<7; x++) {
		for (y=0; y<7; y++) {
			if (board[x+y*7] == choice)
				goto hack;
		}
	}
hack:

	game_state.PlayMove(idx, x, y);
}

void Game::GameResult(int winner) {
	PrintGame();

	Strategy s = Strategy(&game_state);
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

void Game::PrintGame() {
	std::cout << "    ";
	if (idx==0)
		std::cout << HORZ << "US (HORZ)" << CLEAR << "  " << VERT << "THEM" << CLEAR << std::endl;
	else
		std::cout << HORZ << "THEM" << CLEAR << "  " << VERT << "US (VERT)" << CLEAR << std::endl;
	std::cout << std::endl;

	game_state.PrintGameState();
}
