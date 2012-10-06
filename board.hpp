#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <bitset>

class Board {
private:
	int idx;
	int opponent_id;
	std::bitset<7*7> owned_squares_0;
	std::bitset<7*7> owned_squares_1;
	int turn;
	int* board;
	int game_id;
	int our_credits;
	int max_opp_credits;
	float remaining_time;

public:
	Board(
			int idx,
			int opponent_id,
			int* board,
			int game_id,
			int credits,
			float remaining_time
			);
	~Board();
};

#endif
