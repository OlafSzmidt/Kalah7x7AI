#!/bin/python

import sys
import random
from enum import Enum
from board_state import BoardState
from bot_functions import get_legal_moves
from bot_constants import *

# 1 is North, 0 is South
board_side = 0

epsilon = 0.05

log = open("log.txt", "w")

board_state = BoardState()

def get_board_action_hash(state, action, player_board_side):
    feature = 0
    for i in state.south_board_state.keys():
        feature = 31 * feature + state.south_board_state[i]

    feature = 31 * feature + state.south_board_score

    for i in state.north_board_state.keys():
        feature = 31 * feature + state.north_board_state[i]

    feature = 31 * feature + state.north_board_score
    feature = 31 * feature + action
    feature = 31 * feature + board_side

    return feature

def get_feature_vector(state, action, player_board_side):
    features = []

    state_after_action = state.get_next_state(action, player_board_side)

    feature1 = get_board_action_hash(state, action, player_board_side)

    features.append(feature1)

    return features

def decision(prob):
    return random.random() < prob

def dot(K, L):
   if len(K) != len(L):
      return 0

   return sum(i[0] * i[1] for i in zip(K, L))

epsilon = 0.3
weight_vector = []
made_a_move_yet = False

def make_move():
    if decision(epsilon):
        choice = random.choice(get_legal_moves(board_state, board_side))
    else:
        possible_moves = get_legal_moves(board_state, board_side)

        curr_highest_val = 0
        best_move = possible_moves[0]

        for move in possible_moves:
            features = get_feature_vector(board_state, move, board_side)
            value_estimate = dot(features, weight_vector)

            if value_estimate > curr_highest_val:
                best_move = move

        choice = best_move

    move = "MOVE;" + str(choice) + "\n"

    log.write(move)

    sys.stdout.write(move)
    sys.stdout.flush()

    global made_a_move_yet
    made_a_move_yet = True

def on_start(*args):
    """
    Makes a move if SOUTH, otherwise goes back to listening
    """
    global board_side
    if args[0] == 'North':
        board_side = BOARD_SIDE_NORTH
    else:
        board_side = BOARD_SIDE_SOUTH
        make_move()

def on_change(*args):
    move_swap, state, turn = args[0].split(';', 2)

    # Check if <MOVE> or "SWAP"
    if move_swap == 'SWAP':
        global board_side
        board_side = not board_side

    global current_board_state
    board_state_input = state.split(',')

    for i in range(0,7):
        board_state.north_board_state[i + 1] = int(board_state_input[i])

    board_state.north_board_score = board_state_input[NUM_HOLES_PER_SIDE]

    # Input is modular, we need to shift to the next 'part' which starts at
    # index NUM_HOLES_PER_SIDE + 1
    for i in range(NUM_HOLES_PER_SIDE + 1, 2 * NUM_HOLES_PER_SIDE + 1):
        board_state.south_board_state[i - NUM_HOLES_PER_SIDE] = board_state_input[i]

    board_state.south_board_score = board_state_input[2 * NUM_HOLES_PER_SIDE + 1]

    if turn == 'YOU':
        make_move()

    # Ignore OPP turn.

def on_end(*args):
    sys.exit()

message_options = {
    'START': on_start,
    'CHANGE': on_change,
    'END': on_end,
}


def read_message(input):
    """
    Parses message on each input from the game engine and calls the func.
    """
    if input == 'END':
        on_end()

    message_name, message_arguments = input.split(';', 1)

    message_options[message_name](message_arguments)


if __name__ == "__main__":
    while True:
        read_message(raw_input())
