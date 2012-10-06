#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include <bitset>

#include "game.hpp"

class Strategy {
protected:
	GameState *gs;
	static const int MIN = 0;
	static const int MAX = 100000000;

public:
	Strategy(GameState *gs);

	bool is_winning_board_for(int idx);
	bool is_losing_board_for(int idx);

	virtual int evaluate(int idx);
};

#endif
