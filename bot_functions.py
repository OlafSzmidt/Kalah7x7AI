from bot_constants import *

log = open("log.txt", "w")


def get_legal_moves(input_board_state, board_side):
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

    return legal_choices
