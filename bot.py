#!/usr/bin/python3.3

import sys
import random
from board_state import BoardState
from bot_functions import get_legal_moves,min_max

from bot_constants import *

# 1 is North, 0 is South
board_side = 0
made_a_move_yet = False
calls = 0
log = open("bot_log.txt", "w")

board_state = BoardState()



def get_max_value(board_state, board_side, depth):
    global calls
    calls += 1

    curr_highest_val = 0

    possible_moves = get_legal_moves(board_state, board_side, is_second_move=False)

    if len(possible_moves) == 0:
        stones_opp_side = board_state.total_stones_on_side(not board_side)

        board_state_copy = board_state.copy()

        board_state_copy._increase_score_for_side(not board_side, stones_opp_side)

        if board_state_copy.get_score_for_side(board_side) > board_state_copy.get_score_for_side(not board_side):
            return float("inf")
        elif board_state_copy.get_score_for_side(board_side) < board_state_copy.get_score_for_side(not board_side):
            return float("-inf")
        else:
            return 0

    for move in possible_moves:
        next_state = board_state.get_next_state(move, board_side)

        if depth > DEPTH_LIMIT:
            val = get_heuristic_val(next_state, board_side)
        else:
            if board_state.do_we_play_again(move, board_side, is_first_move=False):
                val = get_max_value(next_state, board_side, depth + 1)
            else:
                val = get_min_value(next_state, not board_side, depth + 1, is_second_move=False)

        if val > curr_highest_val:
            curr_highest_val = val


    return curr_highest_val

def get_min_value(board_state, board_side, depth, is_second_move):
    global calls
    calls += 1
    curr_lowest_val = 0

    possible_moves = get_legal_moves(board_state, board_side, is_second_move)

    if len(possible_moves) == 0:
        stones_opp_side = board_state.total_stones_on_side(not board_side)

        board_state_copy = board_state.copy()

        board_state_copy._increase_score_for_side(not board_side, stones_opp_side)

        if board_state_copy.get_score_for_side(board_side) > board_state_copy.get_score_for_side(not board_side):
            return float("-inf")
        elif board_state_copy.get_score_for_side(board_side) < board_state_copy.get_score_for_side(not board_side):
            return float("inf")
        else:
            return 0

    for move in possible_moves:
        next_state = board_state.get_next_state(move, board_side)

        if depth > DEPTH_LIMIT:
            val = get_heuristic_val(next_state, not board_side)
        else:
            if board_state.do_we_play_again(move, board_side, is_first_move=False):
                val = get_min_value(next_state, board_side, depth + 1, is_second_move)
            elif move != SWAP_MOVE:
                val = get_max_value(next_state, not board_side, depth + 1)
            else:
                val = get_max_value(next_state, board_side, depth + 1)

        if val < curr_lowest_val:
            curr_lowest_val = val


    return curr_lowest_val

# def min_max(input_board_state, is_first_move, is_second_move, depth, max_player_side, is_max_player, alpha, beta):

def make_move(is_first_move, is_second_move):
    global calls
    calls = 0
    log.write("Making a move \n")
    log.write("=== Board State === \n")
    log.write(str(board_state) + "\n")
    log.write("=================== \n")

    possible_moves = get_legal_moves(board_state, board_side, is_second_move)

    log.write("=== Legal Moves ===\n")
    log.write(str(possible_moves))
    log.write("===========\n")

    curr_highest_val = 0
    best_move = possible_moves[0]

    for move in possible_moves:
        next_state = board_state.get_next_state(move, board_side)

        if board_state.do_we_play_again(move, board_side, is_first_move):
            value = min_max(next_state, False, False, 1, board_side, True, float("-inf"), float("inf"))
        elif move != SWAP_MOVE:
            value = min_max(next_state, False, is_first_move, 1, board_side, False, float("-inf"), float("inf"))
        else:
            value = min_max(next_state, False, False, 1, not board_side, False, float("-inf"), float("inf"))

        if value > curr_highest_val:
            best_move = move

    choice = best_move

    if choice == -1:
        move = "SWAP\n"
        board_side = not board_side
    else:
        move = "MOVE;" + str(choice) + "\n"

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
        make_move(is_first_move=True, is_second_move=False)

def on_change(*args):
    move_swap, state, turn = args[0].split(';', 2)

    log.write("==== Bot Input ====\n")
    log.write(args[0] + "\n")
    log.write("====================\n")

    # Check if <MOVE> or "SWAP"
    if move_swap == 'SWAP':
        global board_side
        board_side = not board_side

    global current_board_state
    board_state_input = state.split(',')

    for i in range(0,7):
        board_state.north_board_state[i + 1] = int(board_state_input[i])

    board_state.north_board_score = int(board_state_input[NUM_HOLES_PER_SIDE])

    # Input is modular, we need to shift to the next 'part' which starts at
    # index NUM_HOLES_PER_SIDE + 1
    for i in range(NUM_HOLES_PER_SIDE + 1, 2 * NUM_HOLES_PER_SIDE + 1):
        board_state.south_board_state[i - NUM_HOLES_PER_SIDE] = int(board_state_input[i])

    board_state.south_board_score = int(board_state_input[2 * NUM_HOLES_PER_SIDE + 1])


    log.write("==== Loaded State ====\n")
    log.write(str(board_state) + "\n")
    log.write("============\n")

    global made_a_move_yet
    if turn == 'YOU':
        if not made_a_move_yet:
            make_move(is_first_move=False, is_second_move=True)
        else:
            make_move(is_first_move=False, is_second_move=False)

    made_a_move_yet = True

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
    try:
        while True:
            inp = input()
            read_message(inp)
    except:
        log.write(str(sys.exc_info()[3]))
        print(str(sys.exc_info()[3]))
