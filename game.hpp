#ifndef BOARD_HPP
#define BOARD_HPP

#define GET_BIT(_var, _idx) ( !!((_var) & (1UL << (_idx))) )
#define SET_BIT(_var, _idx) ((_var) = (_var) | (1UL << (_idx)))
#define CLR_BIT(_var, _idx) ((_var) &= (~(1UL << (_idx))))

class GameMove;

class GameState {
private:
	const int *board;
	int our_credits;
	int max_opp_credits;
	int turn_number;

	const char* color(int x, int y);
	const char* color_T(int x, int y);

public:
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

public:
	static int idx;

	static const char* CLEAR;
	static const char* HORZ;
	static const char* VERT;
	static const char* WIN;

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
