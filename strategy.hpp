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
	int idx;

	int groups[49];
	std::vector<int> path_node[49];
	std::vector<int> path_groups[49];
	int path_hamming[49];

	int short_path_size;
	int short_path[49];

	void ShortestPath(unsigned &);
	void BuildPath(unsigned long cur_set, int cur_loc, int child_loc);

public:
	static const int weight_map[49];

	GordonStrategy(GameState *gs, int idx);
	void GetMove(int, int*, int&, int&);
};

#endif
