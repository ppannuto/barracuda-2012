#include "board.hpp"

#include <iostream>

Board::Board(
		int idx,
		int opponent_id,
		int* board,
		int game_id,
		int credits,
		float remaining_time
		): idx(idx), opponent_id(opponent_id), board(board), game_id(game_id), our_credits(credits), max_opp_credits(credits), remaining_time(remaining_time) {
}

Board::~Board() {
	std::cout << "idx: " << idx << std::endl;

	delete board;
}
