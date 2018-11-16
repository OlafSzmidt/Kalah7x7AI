import sys
from enum import Enum
from random import randint

# 1 is North, 0 is South
board_side = 0

BOARD_SIDE_SOUTH = 0
BOARD_SIDE_NORTH = 1

NUM_HOLES_PER_SIDE = 7

def make_move():
    random_choice = randint(1, NUM_HOLES_PER_SIDE)
    print("MOVE;" + str(random_choice))

def on_start(*args):
    """
    Makes a move if SOUTH, otherwise goes back to listening
    """
    global board_side
    if args[0] == 'North':
        board_side = BOARD_SIDE_NORTH
        make_move()
    else:
        board_side = BOARD_SIDE_SOUTH

def on_change(*args):
    move_swap, state, turn = args[0].split(';', 2)

    # Check if <MOVE> or "SWAP"
    if move_swap == 'SWAP':
        global board_side
        board_side = not board_side


    global current_state
    current_state = state.split(',')

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
