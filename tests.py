import unittest
from board_state import BoardState

class TestBot(unittest.TestCase):

    def setUp(self):
        self.board_state = BoardState()

    def test_get_next_state(self):
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

        self.assertEquals(next_state.north_board_score, expected_north_score)
        self.assertEquals(next_state.south_board_score, expected_south_score)
        self.assertEquals(next_state.south_board_state, expected_south_state)
        self.assertEquals(next_state.north_board_state, expected_north_state)
