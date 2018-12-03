from bot_constants import *

log = open("function_log.txt", "w")


def get_legal_moves(input_board_state, board_side, is_second_move):
    legal_choices = []

    if board_side == BOARD_SIDE_NORTH:
        for hole, seeds in input_board_state.north_board_state.items():
            if seeds > 0:
                log.write("hole: " + str(hole) + "; seeds: " + str(seeds) + "\n")
                legal_choices.append(hole)
    else:
        for hole, seeds in input_board_state.south_board_state.items():
            if seeds > 0:
                legal_choices.append(hole)

    if is_second_move:
        legal_choices.append(SWAP_MOVE)

    return legal_choices
