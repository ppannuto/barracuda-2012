#include "strategy.hpp"

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

bool Strategy::is_winning_board_for(int idx) {
	std::bitset<7> pots[7] = {0};
	std::bitset<7*7> *cur_set = &gs->owned_squares_row_maj[idx];

	int d1 = (idx) ? 1 : 7;
	int d2 = (idx) ? 7 : 1;

	for (int y = 0; y < 7; y++)
		for (int x = 0; x < 7; x++)
			pots[y][x] = (*cur_set)[x*d1 + y*d2];

	std::bitset<7> cur_pot = 0;
	for (int pot = 0; pot < 6; pot++) {
		for (int i = 0; i < 7; i++)
			cur_pot = (pots[pot] << 1) | pots[pot] | (pots[pot] >> 1);
		cur_pot &= pots[pot+1];
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
