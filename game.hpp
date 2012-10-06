#ifndef BOARD_HPP
#define BOARD_HPP

#include <map>
#include <cassert>

#define GET_BIT(_var, _idx) ( !!((_var) & (1UL << (_idx))) )
#define SET_BIT(_var, _idx) ((_var) = (_var) | (1UL << (_idx)))
#define CLR_BIT(_var, _idx) ((_var) &= (~(1UL << (_idx))))

#define MIN(_a, _b) (( (_a) < (_b) ) ? (_a) : (_b))

class GameMove;

class GameState {
private:
	int our_credits;
	int max_opp_credits;
	int turn_number;

	const char* color(int x, int y);
	const char* color_T(int x, int y);

public:
	const int *board;
	// These two should always be transposes of one another
	unsigned long owned_squares_row_maj[2];
	unsigned long owned_squares_col_maj[2];

	GameState(
			const int *board,
			int our_credits,
			int max_opp_credits,
			int turn_number
		 );
	GameState(
			const GameState& base,
			const GameMove& move,
			int bid
		 );

	void GetMoves(GameMove output[]);

	void PlayMove(int idx, int x, int y);

	void PrintGameState();
};

// Hooray forward decl's
#include "strategy.hpp"

class Game {
private:
	int opponent_id;
	int turn;
	int* board;
	int game_id;
	float remaining_time;

	int next_choice;

	GameState game_state;
	long num_procs;

	Strategy *strategy;

public:
	static int idx;

	static const char* BLACK;
	static const char* RED;
	static const char* GREEN;
	static const char* YELLOW;
	static const char* BLUE;
	static const char* PURPLE;
	static const char* TEAL;
	static const char* GREY;

	static const char* CLEAR;
	static const char* HORZ;
	static const char* VERT;
	static const char* WIN;

	static std::map<int, int> square_to_x;
	static std::map<int, int> square_to_y;

	Game(
			int idx,
			int opponent_id,
			int* board,
			int game_id,
			int credits,
			float remaining_time
			);
	~Game();

	int Bid(int offers_len, int *offers);
	int MakeChoice();
	void MoveResult(int idx, int choice);
	void GameResult(int winner);

	const char* GroupColor(int x, int y);
	void PrintGroups();
	void PrintGame();
};

class GameMove {
public:
	bool IsMyMove() const { return id == Game::idx ; };
	bool IsValid() const { return x >= 0 && y >= 0; };
	int id;       // Player ID.
	int x, y;      // Position to play.
};

#endif
