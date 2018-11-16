import sys
import random
from enum import Enum

# 1 is North, 0 is South
board_side = 0
current_board_state = [7] * 7 + [0] + [7] * 7 + [0]
BOARD_SIDE_SOUTH = 0
BOARD_SIDE_NORTH = 1

NUM_HOLES_PER_SIDE = 7


def get_legal_actions(board_state, board_side):
    """
    :board_state - list
    :board_side - int n in [0, 1]
    """
    legal_choices = []

    if board_side == BOARD_SIDE_NORTH:
        for i in range(0, NUM_HOLES_PER_SIDE):
            if board_state[i] > 0:
                legal_choices.append(i)
    else:
        for i in range(NUM_HOLES_PER_SIDE + 1, 15):
            if board_state[i] > 0:
                legal_choices.append(i)

    return legal_choices


def make_move():
    random_choice = random.choice(get_legal_actions(current_board_state, board_side))
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

    global current_board_state
    current_board_state = state.split(',')

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
