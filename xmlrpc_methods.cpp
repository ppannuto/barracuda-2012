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

// ------------------------------------------------------------------------
// PingMethod
// ------------------------------------------------------------------------

PingMethod::PingMethod() {}
PingMethod::~PingMethod() {}

void PingMethod::execute(
		const xmlrpc_c::paramList& paramList,
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
	int player_id = xmlrpc_c::value_int(gstate["idx"]);

	// The 'board' needs a little transformation:
	std::vector<xmlrpc_c::value> unprocessed_board = xmlrpc_c::value_array(gstate["board"]).vectorValueValue();
	std::vector< std::vector<xmlrpc_c::value> > board;
	for (std::vector<xmlrpc_c::value>::iterator i = unprocessed_board.begin(); i != unprocessed_board.end(); ++i) {
		board.push_back(xmlrpc_c::value_array(*i).vectorValueValue());
	}

	// You can access items from the board like this:
	int someNum = xmlrpc_c::value_int(board[0][0]);

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
	int offer0 = xmlrpc_c::value_int(offer[0]);

	int bid = 0;
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

	int choice = -1;
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
	}

	*retval = xmlrpc_c::value_boolean(true);
}
