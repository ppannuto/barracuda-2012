#include "game_tree.h"

GameTree::GameTree()
{
}

GameTree::~GameTree()
{
}

// Returns a pair containing the best move's number (as returned by
// state.get_moves()). And the value of the best move second.
static Decision_t GameTree::DecideMove(const GameState &state)
{
	GameTree tree();
//	my_bidder = ;
//	opponent_bidder = ;
//	board_evaluator = ;
	return tree.MakeNodeDecision(state, 3);
}

Decision_t GameTree::MakeNodeDecision(const GameState& state, int ply)
{
	const int heuristic_bid_cost = 5;
	const int move_total = 15;
	GameMove moves[move_total];

	// Loop through all possible moves (win, loss, no-move), get score
	// for each scenario.
	state.GetMoves(moves);
	GameTree::Decision_t my_move;
	GameTree::Decision_t opp_move;
	my_move.value = 0;
	opp_move.value = 0;
	for (int i=0; i<move_total; ++i)
	{
		if (!moves[i].IsValid())
			continue;

		GameState new_state(state, moves[i], heuristic_bid_cost);
		double value;
		if (ply == 0)
		{
			value = board_evaluator(new_state);
		}
		else
		{
			GameTree::Decision_t move = MakeNodeDecision(new_state, ply-1);
			value = move.value;
		}
		
		if (moves[i].IsMyMove() && value > my_move.value)
		{
			my_move.value = value;
			my_move.move = moves[i];
		}
		else if (!moves[i].IsMyMove() && value > opp_move.value)
		{
			opp_move.value = value;
			opp_move.move = moves[i];
		}
	}

	// my_move and opp_move now contain optimal move scores for each player.
	// Estimate the outcome of the auction.
	int my_bid = my_bidder(my_move.value, opp_move.value, state.my_credit, state.opp_credit);
	int opp_bid = opponent_bidder(opp_move.value, my_move.value, state.opp_credit, state.my_credit);

	if (my_bid >= opp_bid)
	{
		return my_move;
	}
	else
	{
		return opp_move;
	}
}

