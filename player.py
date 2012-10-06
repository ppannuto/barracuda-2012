#!/usr/bin/python
# vim: ts=4 et sw=4 sts=4

"""This is the dufus player for Python."""

import sys
import random
import numpy
import logging

logging.basicConfig(level=logging.DEBUG)

import SimpleXMLRPCServer

class Game(object):
    CLEAR = '\033[m'
    HORZ  = '\033[0;43m'
    VERT  = '\033[0;44m'
    WIN   = '\033[0;41m'

    def reverse_mapping(self, number):
	    x = self.square_mapping[number] % 7;
	    y = self.square_mapping[number] / 7;
	    return x,y;

    def __init__(self, state):
        self.square_mapping = list([1]*49);
	for i in range(1,49):
		for j in range(1,49):
			x = j % 7;
			y = j / 7;
			if (self.board[x][y] == i):
				self.square_mapping[i] = j;

	self.opp_est_cash = 98;
	self.last_bid = 0;

	self.idx = state['idx']
        self.opp_idx = (self.idx+1)%2
        self.turn = 0
        self.owned_squares = [[], []]
        self.credits = 98
        self.board = state['board']

    def __repr__(self):
        def color(x, y):
            if self.board[x][y] in self.owned_squares[0]:
                return self.HORZ
            elif self.board[x][y] in self.owned_squares[1]:
                return self.VERT
            else:
                return self.CLEAR

        s = ""

        if self.idx == 0:
            s += "     %sUS (HORZ)%s  %sOPP%s  " % (self.HORZ, self.CLEAR, self.VERT, self.CLEAR)
        else:
            s += "     %sOPP%s  %sUS (VERT)%s  " % (self.HORZ, self.CLEAR, self.VERT, self.CLEAR)

        s += '\n\n'

        for x in xrange(7):
            for y in xrange(7):
                s += "%s%3d%s" % (color(x,y), self.board[x][y], self.CLEAR)
            s += '\n'

        return s

    def __str__(self):
        return self.__repr__()

    def move(self, idx, move):
        self.owned_squares[idx].append(move)

    def build_vtree(self):
        self.vtree = numpy.zeros(shape=(7,7), dtype='int')

        for x in xrange(7):
            if self.board[x][6] in self.owned_squares[self.idx]:
                self.vtree[x][6] = 1
            elif self.board[x][6] in self.owned_squares[(self.idx+1)%2]:
                self.vtree[x][6] = -1

        for y in reversed(xrange(6)):
            for x in xrange(7):
                if self.board[x][y] in self.owned_squares[self.idx]:
                    self.vtree[x][y] += 1

                try:
                    self.vtree[x][y] += self.vtree[x-1][y+1]
                except IndexError:
                    pass

                self.vtree[x][y] += self.vtree[x][y+1]

                try:
                    self.vtree[x][y] += self.vtree[x+1][y+1]
                except IndexError:
                    pass

                if self.board[x][y] in self.owned_squares[self.opp_idx]:
                    self.vtree[x][y] = -1

    def print_vtree(self):
        s = ""
        for y in xrange(7):
            for x in xrange(7):
                s += "%5d" % (self.vtree[x][y])
            s += '\n'
        print s

    def calc_bid(self, offer):
		bid = floor(min(opponent_path_average_value(), player_path_average_value())) - 1;
		if (self.player_can_win(offer, self.idx)):
			bid = self.credits;
		elif (self.player_can_win(offer, (self.idx+1)%2) and self.credits >= self.opp_est_cash+1):
			bid = self.opp_est_cash+1;
		return bid;

    def opponent_path_average_value(self):
		path = shortest_path(self.board, (self.idx+1)%2);
		owned_count = 0;
		for x,y in path:
			if (self.board[x][y] in self.owned_squares[idx]):
				owned_count = owned_count + 1;
		return self.opp_est_cash / (len(path) - owned_count);

    def player_path_average_value(self):
		path = shortest_path(self.board, self.idx);
		owned_count = 0;
		for x,y in path:
			if (self.board[x][y] in self.owned_squares[idx]):
				owned_count = owned_count + 1;
		return self.credits / (len(path) - owned_count);
	
    def player_can_win(self, offer, idx):
	    	path = shortest_path(self.board, idx);
#		a_seen = false;
#		b_seen = false;
		owned_count = 0;
		used_freebie = false;
		for x,y in path:
#			if (self.VERT == self.idx && y == 0 || self.HORZ == self.idx && x == 0)
#				a_seen = true;
#			if (self.VERT == self.idx && y == 6 || self.HORZ == self.idx && y == 6)
#				b_seen = true;

			if (self.board[x][y] in self.owned_squares[idx]):
				owned_count = owned_count + 1;
			elif (not used_freebie and self.board[x][y] in offer):
				owned_count = owned_count + 1;
				used_freebie = true;
		return (owned_count == len(path));
#		return (owned_count == len(path) && a_seen && b_seen);

def shortest_path(board, player_id, turn):
	return ((0,0), (1,0), (2,0), (3,0), (4,0), (5,0), (6,0));

game = None

funcs = []
def serve(f):
    funcs.append(f)
    return f

@serve
def ping():
    return True

@serve
def init_game(state):
    try:
        print state
        global game
        game = Game(state)
    except Exception as e:
        logging.exception('init_game')
        raise e

@serve
def get_bid(offer, state):
    try:
        #print "Get bid: %s %s" % (offer, state)
	global game
	bid = game.calc_bid(offer);
	game.last_bid = bid;
        return bid;
    except Exception as e:
        logging.exception('get_bid')
        raise e


@serve
def make_choice(offer, state):
    try:
        #print "Make choice: %s %s" % (offer, state)
        return random.choice(offer)
    except Exception as e:
        logging.exception('make_choice')
        raise e

@serve
def move_result(result):
    try:
        #print "Move result: %s" % result
        global game
        if result['result'] == 'you_chose':
            game.move(game.idx, result['choice'])
        elif result['result'] == 'opponent_chose':
            game.move((game.idx+1)%2, result['choice'])
	    self.opp_est_cash = self.opp_est_cash - (self.last_bid + 1);
        return False
    except Exception as e:
        logging.exception('move_result')
        raise e

@serve
def game_result(result):
    try:
        #print "Game result: %s" % result
        global game
        print
        print game
        print
        print "  Winner:", result['winner']
        print
        logging.debug('building tree...')
        game.build_vtree()
        logging.debug('printing tree...')
        game.print_vtree()
        print 'done'
        return False
    except Exception as e:
        logging.exception('game_result')
        raise e

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "Usage: %s [portnum] to start the dufus on the given port" % sys.argv[0]
    else:
        port = int(sys.argv[1])
        # logRequests must be false, because otherwise the logger tries to
        # reverse DNS lookup every request, which is too slow.
        server = SimpleXMLRPCServer.SimpleXMLRPCServer(
            ('', int(port)), logRequests = False)

        for f in funcs:
            server.register_function(f)

        print "Running Python Dufus"
        server.serve_forever()

