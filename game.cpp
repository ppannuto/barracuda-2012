#include "game.hpp"

#include <iostream>

GameState::GameState(
		const int* board,
		int our_credits,
		int max_opp_credits):
	board(board),
	our_credits(our_credits),
	max_opp_credits(max_opp_credits) {
		;
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
	idx(idx),
	opponent_id(opponent_id),
	turn(0),
	board(board),
	game_id(game_id),
	remaining_time(remaining_time),
	game(GameState(board, credits, credits)) {
}

Game::~Game() {
	std::cout << "idx: " << idx << std::endl;

	delete board;
}
