#ifndef GAME_TREE_H
#define GAME_TREE_H

class GameState;

// Returns how much to bid given win board score, loss board score,
// estimated current credit, estimated opponent credit.
typedef int (*BidFunction)(double, double, int, int);

// Returns board score
typedef double (*BoardValueFunction)(const GameState&);

// Holds the move made and the expected final outcome resulting from move.
typedef Decision_t std::pair<GameMove, double> Decision_t;

class GameTree
{
public:
GameTree();
~GameTree();

// Returns how much to bid and which move to make upon winning.
static std::pair<int, GameMove> DecideMove(const GameState &);

private:
static BidFunction my_bidder, opponent_bidder;
static BoardValueFunction board_evaluator;

struct Decision_t {
	GameMove move;
	double value;
};

Decision_t MakeNodeDecision(const GameState& state, int ply);
};

#endif
