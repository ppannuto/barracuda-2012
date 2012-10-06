#ifndef BOARD_HPP
#define BOARD_HPP

#include <bitset>

class GameState {
private:
	const int *board;
	std::bitset<7*7> owned_squares_0;
	std::bitset<7*7> owned_squares_1;
	int our_credits;
	int max_opp_credits;

public:
	GameState(
			const int *board,
			int our_credits,
			int max_opp_credits
		 );
};

class Game {
private:
	int idx;
	int opponent_id;
	int turn;
	int* board;
	int game_id;
	float remaining_time;

	GameState game;

	static const char* CLEAR;
	static const char* HORZ;
	static const char* VERT;
	static const char* WIN;

public:
	Game(
			int idx,
			int opponent_id,
			int* board,
			int game_id,
			int credits,
			float remaining_time
			);
	~Game();
};

#endif
