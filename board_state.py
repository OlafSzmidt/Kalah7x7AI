from bot_functions import get_legal_moves
from bot_constants import *

log = open("log.txt", "w")

class Hole:
    def __init__(self, number, board_side):
        self.number = number
        self.board_side = board_side


class BoardState:
    def __init__(self):
        self.north_board_score = 0
        self.south_board_score = 0
        self.north_board_state = {1:7, 2:7, 3:7, 4:7, 5:7, 6:7, 7:7}
        self.south_board_state = {1:7, 2:7, 3:7, 4:7, 5:7, 6:7, 7:7}

    def copy(self):
        new_board_state = BoardState()
        new_board_state.north_board_score = self.north_board_score
        new_board_state.south_board_score = self.south_board_score
        new_board_state.north_board_state = dict(self.north_board_state)
        new_board_state.south_board_state = dict(self.south_board_state)

        return new_board_state

    def get_board_state_for_side(self, board_side):
        if board_side == BOARD_SIDE_NORTH:
            return self.north_board_state
        return self.south_board_state

    def _increase_score_for_side(self, board_side):
        if board_side == BOARD_SIDE_NORTH:
            self.north_board_score += 1
        else:
            self.south_board_score += 1

    def get_next_state(self, hole_choice, player_board_side):
        next_state = self.copy()

        if hole_choice in get_legal_moves(self, player_board_side):
            side_state = next_state.get_board_state_for_side(player_board_side)

            remaining_seeds = side_state[hole_choice]
            # Empty the selected hole
            side_state[hole_choice] = 0

            # Picked any hole but the last one before the scoring well
            if hole_choice < 7:
                next_hole = Hole(hole_choice + 1, player_board_side)
            else:
                # We chose the /final/ hole on the players side.
                # Next hole is past the scoring well, so we switch sides and
                # decrement the remaining seeds (1 goes to scoring well)
                next_hole = Hole(1, not player_board_side)
                remaining_seeds -= 1
                next_state._increase_score_for_side(player_board_side)

            while remaining_seeds > 0:
                # Get state for the correct side
                next_hole_board_state = next_state.get_board_state_for_side(next_hole.board_side)
                # Increment the number of seeds in the next hole
                next_hole_board_state[next_hole.number] += 1
                remaining_seeds -= 1

                # Now calculate the next hole
                previous_hole = next_hole
                if remaining_seeds != 0:
                    next_hole_number = previous_hole.number + 1
                    next_hole_side = previous_hole.board_side

                    # Check if need to change sides
                    if next_hole_number > 7:
                        next_hole_number = 1
                        next_hole_side = not next_hole_side

                        # If we left the players side
                        if previous_hole.board_side == player_board_side:
                            next_state._increase_score_for_side(player_board_side)
                            remaining_seeds -= 1

                next_hole = Hole(next_hole_number, next_hole_side)
        else:
            log.write("Action is not a legal move: " + str(hole_choice) + "\n")
            sys.stderr.write("CHECK LOGS! EXITING")

            sys.exit(0)

        return next_state
