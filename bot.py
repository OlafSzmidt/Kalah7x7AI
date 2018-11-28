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

def make_move():
    random_choice = random.choice(get_legal_moves(board_state, board_side))
    move = "MOVE;" + str(random_choice) + "\n"

    log.write(move)

    sys.stdout.write(move)
    sys.stdout.flush()

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

while True:
    read_message(raw_input())
