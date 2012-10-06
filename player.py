#!/usr/bin/python
# vim: ts=4 et sw=4 sts=4

"""This is the dufus player for Python."""

import sys
import random
import numpy
import logging
from copy import deepcopy

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
        self.turn = (self.turn + 1) % 7

    def get_bid(self, offer):
        self.next_move = offer[0]

        s = ""
        for x in xrange(7):
            for y in xrange(7):
                s += "%3d" % (self.board[x][y] / 7)
            s += '\n'
        #print s
        self.shortest_path()
	#print self.paths
        return 2

    def process_sp_child(self, cur_x, cur_y, child_x, child_y):
        if child_x < 0 or child_x > 6:
            raise IndexError
        if self.board[cur_x][cur_y] in self.owned_squares[self.idx]:
            for i in xrange(len(self.paths[child_x][child_y])):
                path = self.paths[child_x][child_y][i]
                group = self.groups[child_x][child_y][i]
                cand = deepcopy(group)
                if cand not in self.groups[cur_x][cur_y]:
                    try:
                        if len(cand) < len(self.groups[cur_x][cur_y]):
                            self.paths[cur_x][cur_y] = [[]]
                            self.groups[cur_x][cur_y] = [[]]
                            self.paths[cur_x][cur_y].append(deepcopy(path))
                            self.paths[cur_x][cur_y][-1].append((cur_x, cur_y))
                            self.groups[cur_x][cur_y].append(cand)
                        elif len(cand) == len(self.groups[cur_x][cur_y]):
                            self.paths[cur_x][cur_y].append(deepcopy(path))
                            self.paths[cur_x][cur_y][-1].append((cur_x, cur_y))
                            self.groups[cur_x][cur_y].append(cand)
                    except IndexError:
                        self.paths[cur_x][cur_y].append(deepcopy(path))
                        self.paths[cur_x][cur_y][-1].append((cur_x, cur_y))
                        self.groups[cur_x][cur_y].append(cand)
        else:
            #print "child_x", child_x
            #print "child_y", child_y
            #print len(self.paths[child_x][child_y])
            #print self.paths[child_x][child_y]
            #print self.groups[child_x][child_y]
            for i in xrange(len(self.paths[child_x][child_y])):
                path = self.paths[child_x][child_y][i]
                group = self.groups[child_x][child_y][i]
                if self.board[cur_x][cur_y] / 7 in group:
                    continue
                cand = deepcopy(group)
                cand.append(self.board[cur_x][cur_y] / 7)
                #print '---'
                #print self.board[cur_x][cur_y]/7
                #print group
                #print '---'
                if cand not in self.groups[cur_x][cur_y]:
                    try:
                        if len(cand) < len(self.groups[cur_x][cur_y][0]):
                            self.paths[cur_x][cur_y] = [[]]
                            self.groups[cur_x][cur_y] = [[]]
                            self.paths[cur_x][cur_y].append(deepcopy(path))
                            self.paths[cur_x][cur_y][-1].append((cur_x, cur_y))
                            self.groups[cur_x][cur_y].append(cand)
                        elif len(cand) == len(self.groups[cur_x][cur_y][0]):
                            self.paths[cur_x][cur_y].append(deepcopy(path))
                            self.paths[cur_x][cur_y][-1].append((cur_x, cur_y))
                            self.groups[cur_x][cur_y].append(cand)
                        else:
                            pass
                    except IndexError:
                        self.paths[cur_x][cur_y].append(deepcopy(path))
                        self.paths[cur_x][cur_y][-1].append((cur_x, cur_y))
                        self.groups[cur_x][cur_y].append(cand)
                else:
                    pass




    def shortest_path(self):
        self.paths = [[[] for y in xrange(7)] for x in xrange(7)]
        self.groups = [[[] for y in xrange(7)] for x in xrange(7)]

        for x in xrange(7):
            y = 6
            self.paths[x][y] = [[(x,y)]]
            self.groups[x][y] = [[self.board[x][y]/7]]

        for y in reversed(xrange(6)):
            for x in xrange(7):
                try:
                    self.process_sp_child(x, y, x-1, y+1)
                except IndexError:
                    pass
                self.process_sp_child(x, y, x, y+1)
                try:
                    self.process_sp_child(x, y, x+1, y+1)
                except IndexError:
                    pass

        min_path = list([10]*10)
        for x in xrange(7):
            y = 0
            for path in self.paths[x][y]:
                if len(path) < len(min_path):
                    min_path = path

        if len(min_path) == 10:
            print "FUCKETY FUCK"

        #print self.paths
        #print "Path 0, 0"
        #print self.paths[0][0]
        #print "Path 0, 5"
        #print self.paths[0][5]
        #print "Path 5, 0"
        #print self.paths[5][0]
        print'Min path:'
        print min_path
        return min_path


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
        #print state
        global game
        game = Game(state)
        print game
    except Exception as e:
        logging.exception('init_game')
        raise e

@serve
def get_bid(offer, state):
    try:
        #print "Get bid: %s %s" % (offer, state)
        global game
        game.get_bid(offer)

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
        return game.next_move
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

