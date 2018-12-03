import unittest
from board_state import BoardState
from bot import make_move
from bot import board_side
from bot_functions import get_legal_moves

class TestBot(unittest.TestCase):

    # Helper non-test method
    def is_expected_board_state(self, board_state, south_state, north_state, south_score, north_score):
        self.assertEquals(board_state.north_board_score, north_score)
        self.assertEquals(board_state.south_board_score, south_score)
        self.assertEquals(board_state.south_board_state, south_state)
        self.assertEquals(board_state.north_board_state, north_state)

    def test_get_next_state(self):
        self.board_state = BoardState()
        self.assertEquals(self.board_state.north_board_score, 0)
        self.assertEquals(self.board_state.south_board_score, 0)
        self.assertEquals(self.board_state.south_board_state, {1:7, 2:7, 3:7, 4:7, 5:7, 6:7, 7:7})
        self.assertEquals(self.board_state.north_board_state, {1:7, 2:7, 3:7, 4:7, 5:7, 6:7, 7:7})


        chosen_hole = 1

        # SOUTH is 0
        next_state = self.board_state.get_next_state(chosen_hole, 0)

        expected_south_state = {1:0, 2:8, 3:8, 4:8, 5:8, 6:8, 7:8}
        expected_north_state = {1:7, 2:7, 3:7, 4:7, 5:7, 6:7, 7:7}
        expected_south_score = 1
        expected_north_score = 0

        self.is_expected_board_state(next_state, expected_south_state, expected_north_state,
                                     expected_south_score, expected_north_score)

    def test_pie_rule(self):
        self.board_state = BoardState()
        self.board_state = self.board_state.get_next_state(1, 0)

        pie_rule_value = -1
        next_state = self.board_state.get_next_state(pie_rule_value, 1)

        expected_south_state = {1:7, 2:7, 3:7, 4:7, 5:7, 6:7, 7:7}
        expected_north_state = {1:0, 2:8, 3:8, 4:8, 5:8, 6:8, 7:8}
        expected_south_score = 0
        expected_north_score = 1

        self.is_expected_board_state(next_state, expected_south_state, expected_north_state,
                                     expected_south_score, expected_north_score)

    def test_get_legal_moves_start(self):
        self.board_state = BoardState()
        legal_moves = get_legal_moves(self.board_state, 0)

        moves_map = {}
        for move in legal_moves:
            # Should not have duplicates in legal moves list
            self.assertFalse(move in moves_map)
            moves_map[move] = True;

        expected_moves_map = {1:True, 2:True, 3:True, 4:True, 5:True, 6:True, 7:True}
        for expected_move in expected_moves_map.keys():
            moves_map.pop(expected_move)

        # legal_moves should have the exact same items as expected_moves_map
        self.assertEquals(len(moves_map), 0)

    def test_get_legal_moves_empty(self):
        self.board_state = BoardState()
        self.board_state.north_board_score = 0
        self.board_state.south_board_score = 0
        self.board_state.north_board_state = {1:0, 2:0, 3:0, 4:0, 5:0, 6:0, 7:0}
        self.board_state.south_board_state = {1:0, 2:0, 3:0, 4:0, 5:0, 6:0, 7:0}

        legal_moves = get_legal_moves(self.board_state, 0)
        self.assertEquals(len(legal_moves), 0)

    def test_get_legal_moves_one(self):
        self.board_state = BoardState()
        self.board_state.north_board_score = 0
        self.board_state.south_board_score = 0
        self.board_state.north_board_state = {1:0, 2:0, 3:0, 4:0, 5:0, 6:0, 7:0}
        self.board_state.south_board_state = {1:0, 2:0, 3:0, 4:0, 5:0, 6:0, 7:1}

        legal_moves = get_legal_moves(self.board_state, 0)
        self.assertEquals(len(legal_moves), 1)
        self.assertEquals(legal_moves[0], 7)

    def test_make_move(self):
        make_move()
        global board_side
        board_side = not board_side
        make_move()
