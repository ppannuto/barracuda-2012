//
// T-1000
// John Richards <jrichards@barracuda.com>
//

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

#include "xmlrpc_methods.hpp"

#include "game.hpp"
static Game* current_game;

// ------------------------------------------------------------------------
// PingMethod
// ------------------------------------------------------------------------

PingMethod::PingMethod() {}
PingMethod::~PingMethod() {}

void PingMethod::execute(
		const xmlrpc_c::paramList& paramList __attribute__ ((unused)),
		xmlrpc_c::value* const retval) {
	*retval = xmlrpc_c::value_boolean(true);
}

// ------------------------------------------------------------------------
// InitGameMethod
// ------------------------------------------------------------------------

InitGameMethod::InitGameMethod() {}
InitGameMethod::~InitGameMethod() {}

void InitGameMethod::execute(
		const xmlrpc_c::paramList& paramList,
		xmlrpc_c::value* const retval) {
	std::map<std::string, xmlrpc_c::value> gstate = paramList.getStruct(0);

	// Access params like this:
	//int player_id = xmlrpc_c::value_int(gstate["idx"]);

	// The 'board' needs a little transformation:
	std::vector<xmlrpc_c::value> unprocessed_board = xmlrpc_c::value_array(gstate["board"]).vectorValueValue();
	std::vector< std::vector<xmlrpc_c::value> > board;
	for (std::vector<xmlrpc_c::value>::iterator i = unprocessed_board.begin(); i != unprocessed_board.end(); ++i) {
		board.push_back(xmlrpc_c::value_array(*i).vectorValueValue());
	}

	// You can access items from the board like this:
	//int someNum = xmlrpc_c::value_int(board[0][0]);

	int* real_fucking_array = new int[49];
	for (int y = 0; y < 7; y++) {
		for (int x = 0; x < 7; x++) {
			real_fucking_array[x + y*7] = xmlrpc_c::value_int(board[x][y]);
		}
	}

	current_game = new Game(
			xmlrpc_c::value_int(gstate["idx"]),
			xmlrpc_c::value_int(gstate["opponent_id"]),
			real_fucking_array,
			xmlrpc_c::value_int(gstate["id"]),
			xmlrpc_c::value_int(gstate["credits"]),
			xmlrpc_c::value_double(gstate["remaining_time"])
			);

	*retval = xmlrpc_c::value_boolean(true);
}

// ------------------------------------------------------------------------
// GetBidMethod
// ------------------------------------------------------------------------

GetBidMethod::GetBidMethod() {}
GetBidMethod::~GetBidMethod() {}

void GetBidMethod::execute(
		const xmlrpc_c::paramList& paramList,
		xmlrpc_c::value* const retval) {
	std::vector<xmlrpc_c::value> offer = paramList.getArray(0);
	std::map<std::string, xmlrpc_c::value> gstate = paramList.getStruct(1);

	// See InitGameMethod::execute for info on how to use params.
	// Access the offer vector like this:
	//int offer0 = xmlrpc_c::value_int(offer[0]);

	int offers[6];
	int offers_len = offer.size();

	for (int i=0; i < offers_len; i++)
		offers[i] = xmlrpc_c::value_int(offer[i]);

	int bid = current_game->Bid(offers_len, offers);

	*retval = xmlrpc_c::value_int(bid);
}

// ------------------------------------------------------------------------
// MakeChoiceMethod
// ------------------------------------------------------------------------

MakeChoiceMethod::MakeChoiceMethod() {}
MakeChoiceMethod::~MakeChoiceMethod() {}

void MakeChoiceMethod::execute(
		const xmlrpc_c::paramList& paramList,
		xmlrpc_c::value* const retval) {
	std::vector<xmlrpc_c::value> offer = paramList.getArray(0);
	std::map<std::string, xmlrpc_c::value> gstate = paramList.getStruct(1);

	// See InitGameMethod::execute and GetBidMethod::execute for info on how to use params.

	int choice = current_game->MakeChoice();

	*retval = xmlrpc_c::value_int(choice);
}

// ------------------------------------------------------------------------
// MoveResultMethod
// ------------------------------------------------------------------------

MoveResultMethod::MoveResultMethod() {}
MoveResultMethod::~MoveResultMethod() {}

void MoveResultMethod::execute(
		const xmlrpc_c::paramList& paramList,
		xmlrpc_c::value* const retval) {
	std::map<std::string, xmlrpc_c::value> result = paramList.getStruct(0);

	// Use the result struct like you would use the game state struct.
	// See other execute methods for more info.
	std::string result_string = xmlrpc_c::value_string(result["result"]);
	if (result_string == "you_chose") {
		int choice = xmlrpc_c::value_int(result["choice"]);
		current_game->MoveResult(current_game->idx, choice);
	} else if (result_string == "opponent_chose") {
		int choice = xmlrpc_c::value_int(result["choice"]);
		current_game->MoveResult((current_game->idx+1)%2, choice);
	} else {
		current_game->MoveResult(-1, -1);
	}


	*retval = xmlrpc_c::value_boolean(true);
}

// ------------------------------------------------------------------------
// GameResultMethod
// ------------------------------------------------------------------------

GameResultMethod::GameResultMethod() {}
GameResultMethod::~GameResultMethod() {}

void GameResultMethod::execute(
		const xmlrpc_c::paramList& paramList,
		xmlrpc_c::value* const retval) {
	std::map<std::string, xmlrpc_c::value> result = paramList.getStruct(0);

	delete current_game;

	*retval = xmlrpc_c::value_boolean(true);
}
