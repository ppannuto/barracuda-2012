#include "strategy.hpp"
#include "game.hpp"

#include <iostream>
#include <string.h>

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

// ----------------------------------------------------------------------------
const int GordonStrategy::weight_map[49] = {
	0,1,2,3,2,1,0,
	1,2,3,4,3,2,1,
	2,3,4,5,4,3,2,
	3,4,5,6,5,4,3,
	2,3,4,5,4,3,2,
	1,2,3,4,3,2,1,
	0,1,2,3,2,1,0};

GordonStrategy::GordonStrategy(GameState *gs, int idx) : Strategy(gs), idx(idx) {
	unsigned long cur_set = (idx) ? gs->owned_squares_col_maj[idx] : gs->owned_squares_row_maj[idx];

	for (int x=0; x < 7; x++) {
		for (int y=0; y<7; y++) {
			int g = gs->board[x+7*y];
			groups[x+7*y] =
				(0 <= g && g < 7) ? 0 :
				(7 <= g && g < 14) ? 1 :
				(14 <= g && g < 21) ? 2 :
				(21 <= g && g < 28) ? 3 :
				(28 <= g && g < 35) ? 4 :
				(35 <= g && g < 42) ? 5 : 6;
		}
	}

	int x,y;
	for (x=0; x<7; x++) {
		y = 6;
		int my_loc = x+7*y;

		path_node[my_loc].push_back(-1);
		path_groups[my_loc].push_back(1 << groups[my_loc]);
	}
	for (y=5; y>=0; y--) {
		x = 0;
		int my_loc = x+7*y;
		int my_g = groups[my_loc];

		BuildPath(cur_set, my_loc, x+7*(y+1));
		BuildPath(cur_set, my_loc, x+1+7*(y+1));

		for (x=1; x<6; x++) {
			my_loc = x+7*y;
			my_g = groups[my_loc];

			BuildPath(cur_set, my_loc, x-1+7*(y+1));
			BuildPath(cur_set, my_loc, x+7*(y+1));
			BuildPath(cur_set, my_loc, x+1+7*(y+1));
		}

		x = 6;
		my_loc = x+7*y;
		my_g = groups[my_loc];

		BuildPath(cur_set, my_loc, x-1+7*(y+1));
		BuildPath(cur_set, my_loc, x+7*(y+1));
	}

	/*
	for (x=0; x<7; x++) {
		int y = 6;
		lengths[x+7*y] = 1;
	}
	for (y=5; y>=0; y--) {
		for (x=0; x<7; x++) {
			int min = 10;
			for (unsigned i=0; i<paths[x+y*7].size(); i++) {
//				std::cout << "x " << x << " y " << y << " i " << i << " paths[x+y*7][i] " << paths[x+y*7][i] << std::endl;
//				std::cout << "min: " << min << " vs " << lengths[paths[x+y*7][i]] << std::endl;
				int child_cost = GET_BIT(cur_set, paths[x+y*7][i]) ? 0 : 1;
//				std::cout << "child_cost " << child_cost << std::endl;
				min = MIN(min, lengths[paths[x+y*7][i]] + child_cost);
			}
			lengths[x+y*7] = min;
		}
	}

	for (int i=0; i<7; i++)
		top_down_lengths[i] = lengths[i];
	for (int i=7; i<49; i++)
		top_down_lengths[i] = 100;
	for (x=0; x<7; x++) {
		int y=0;
		SetChildLengths(x,y);
	}

	for (y=0; y<7; y++) {
		for (x=0; x<7; x++) {
			std::cout << x+y*7 << ": ";
			for (unsigned i=0; i < paths[x+y*7].size(); i++)
				std::cout << paths[x+y*7][i] << " ";
			std::cout << " (len " << lengths[x+y*7] << ")";
			std::cout << " (top-down len " << top_down_lengths[x+y*7] << ")";
			std::cout << std::endl;
		}
	}
	*/
}

void GordonStrategy::BuildPath(const unsigned long cur_set, int cur_coord, int child_coord) {
	// If we have played here
	if (GET_BIT(cur_set, cur_coord)) {
		for (unsigned i=0; i<path_groups[child_coord].size(); i++) {
			// Make sure an identical path has not been added from a previous child
			int candidate_group = (path_groups[child_coord][i]);
			unsigned j;
			for (j=0; j<path_groups[cur_coord].size(); j++) {
				if (path_groups[cur_coord][j] == candidate_group)
					break; // Found duplicate
			}
			if (j == path_groups[cur_coord].size()) {
				if (__builtin_popcount(candidate_group) < path_hamming[cur_coord]) {
					path_hamming[cur_coord] = __builtin_popcount(candidate_group);
					path_node[cur_coord].clear();
					path_groups[cur_coord].clear();
				}

				// Note - NOT else if - this will be true for the first entry after a clear as well
				if (__builtin_popcount(candidate_group == path_hamming[cur_coord])) {
					path_node[cur_coord].push_back(child_coord);
					path_groups[cur_coord].push_back(candidate_group);
				}
			}
		}
	}
	// We have not played here
	else {
		for (unsigned i=0; i<path_groups[child_coord].size(); i++) {
			// For each of the child's group bitfields...
			if (  !( (1 << groups[cur_coord]) & (path_groups[child_coord][i]) )  ) {
				// If we are NOT in that bitfield, make sure an identical path has not been added from a previous child
				int candidate_group = (1 << groups[cur_coord]) | (path_groups[child_coord][i]);
				unsigned j;
				for (j=0; j<path_groups[cur_coord].size(); j++) {
					if (path_groups[cur_coord][j] == candidate_group)
						break; // Found duplicate
				}
				if (j == path_groups[cur_coord].size()) {
					if (__builtin_popcount(candidate_group) < path_hamming[cur_coord]) {
						path_hamming[cur_coord] = __builtin_popcount(candidate_group);
						path_node[cur_coord].clear();
						path_groups[cur_coord].clear();
					}

					// Note - NOT else if - this will be true for the first entry after a clear as well
					if (__builtin_popcount(candidate_group == path_hamming[cur_coord])) {
						path_node[cur_coord].push_back(child_coord);
						path_groups[cur_coord].push_back(candidate_group);
					}
				}
			}
		}
	}
}

/*
void GordonStrategy::SetChildLengths(int x, int y) {
//	std::cout << "setting (" << x << "," << y << ")" << std::endl;
	for (unsigned i=0; i<paths[x+y*7].size(); i++) {
		if (top_down_lengths[paths[x+y*7][i]] > top_down_lengths[x+y*7]) {
			top_down_lengths[paths[x+y*7][i]] = top_down_lengths[x+y*7];
//			std::cout << "recursing on " << paths[x+y*7][i] << std::endl;
			SetChildLengths(paths[x+y*7][i] % 7, paths[x+y*7][i] / 7);
		}
	}
}
*/

void GordonStrategy::GetMove(
		int offers_len, int *offers,
		int &bid, int &next_move) {
	bid = 14;

	/*
	std::vector<int> candidates;
	int min_len = 10;

	for (int i=0; i<offers_len; i++) {
		int len = lengths[offers[i]];
		if (len < min_len) {
			candidates.clear();
			candidates.push_back(offers[i]);
		} else if (len == min_len) {
			candidates.push_back(offers[i]);
		}
	}

	assert(candidates.size());

	if (candidates.size() == 1) {
		next_move = candidates[0];
		return;
	}

	int max = -1;
	for (unsigned i=0; i<candidates.size(); i++) {
		int x = Game::square_to_x[candidates[i]];
		int y = Game::square_to_y[candidates[i]];
		if (weight_map[x+7*y] > max) {
			max = weight_map[x+7*y];
			next_move = candidates[i];
		}
	}
	*/
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
