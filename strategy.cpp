#include "strategy.hpp"
#include "game.hpp"

#include <iostream>

Strategy::Strategy(GameState *gs) : gs(gs) {
}

int Strategy::evaluate(int idx) {
	if(is_winning_board_for(idx))
		return MAX;

	if(is_losing_board_for(idx))
		return MIN;

	// Calculate budget
	// advanced_calc();
	return 0;
}

static void bin_print(unsigned long bin) {
	for (int i=0; i<7; i++)
		std::cout << GET_BIT(bin, i);
	std::cout << std::endl;
}

bool Strategy::is_winning_board_for(int idx) {
	unsigned long cur_set = (idx) ? gs->owned_squares_col_maj[idx] : gs->owned_squares_row_maj[idx];

	unsigned long cur_pot = (cur_set & 0x7f);
	for (int i = 1; i < 6; i++) {
		//std::cout << "starting cur_pot: "; bin_print(cur_pot);
		cur_pot |= (((cur_pot << 1) | (cur_pot >> 1)) & 0x7f);
		//std::cout << "twiddled cur_pot: "; bin_print(cur_pot);
		cur_pot &= (cur_set >> (7*i)) & 0x7f;
		//std::cout << "   next set bits: "; bin_print((cur_set >> (7*i)) & 0x7f);
		//std::cout << "   anded cur_pot: "; bin_print(cur_pot);
	}

	return cur_pot != 0;
}

bool Strategy::is_losing_board_for(int idx) {
	return is_winning_board_for((idx+1)%2);
}

/*
// Is line contained by x,y blocked going south/east depending on player
bool is_blocked(int player, int x, int y) {
	return false;
}

bool is_connected(int player, int x, int y) {
	return false;
}


class StrategyA: public Strategy {
	public:
		StrategyA(Gamestate *gs) Strategy(gamestate) {}

	private:
		int advanced_calc() {
			return 0;
		}
}  
*/
