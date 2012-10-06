#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include <map>
#include <vector>

class Strategy;
class GordonStrategy;

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

	virtual void GetMove(
			int offers_len, int *offers,
			int &bid, int &next_move
			) {bid = 1; next_move = offers[0];};
};

class GordonStrategy : public Strategy {
private:
	int groups[49];
	std::vector<int> paths[49];
	int lengths[49];
	int top_down_lengths[49];

	void SetChildLengths(int x, int y);

public:
	static const int weight_map[49];

	GordonStrategy(GameState *gs);
	void GetMove(int, int*, int&, int&);
};

#endif
