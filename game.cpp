#include "game.hpp"

#include <iostream>
#include <cassert>
#include <stdio.h>

GameState::GameState(
		const int* board,
		int our_credits,
		int max_opp_credits):
	board(board),
	our_credits(our_credits),
	max_opp_credits(max_opp_credits) {
		;
}

const char* GameState::color(int x, int y) {
	if (owned_squares_0[x+7*y])
		return Game::HORZ;
	else if (owned_squares_1[x+7*y])
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
}

const char *Game::CLEAR = "\033[m";
const char *Game::HORZ  = "\033[0;43m";
const char *Game::VERT  = "\033[0;44m";
const char *Game::WIN   = "\033[0;41m";

Game::Game(
		int idx,
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
	game_state(GameState(board, credits, credits)),
	// public
	idx(idx) {
}

Game::~Game() {
	PrintGame();
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

	if (idx == 0) {
		game_state.owned_squares_0[x+y*7] = 1;
	} else {
		game_state.owned_squares_1[x+y*7] = 1;
	}
}

void Game::PrintGame() {
	std::cout << "    ";
	if (idx)
		std::cout << HORZ << "US (HORZ)" << CLEAR << "  " << VERT << "THEM" << CLEAR << std::endl;
	else
		std::cout << HORZ << "THEM" << CLEAR << "  " << VERT << "US (VERT)" << CLEAR << std::endl;
	std::cout << std::endl;

	game_state.PrintGameState();
}
