from bot_constants import *

log = open("function_log.txt", "w")


def get_legal_moves(input_board_state, board_side, is_second_move):
    legal_choices = []

    if board_side == BOARD_SIDE_NORTH:
        for hole, seeds in input_board_state.north_board_state.items():
            if seeds > 0:
                legal_choices.append(hole)
    else:
        for hole, seeds in input_board_state.south_board_state.items():
            if seeds > 0:
                legal_choices.append(hole)

    if is_second_move:
        legal_choices.append(SWAP_MOVE)

    return legal_choices


def get_heuristic_val(board_state, max_player_side):
    max_score = board_state.get_score_for_side(max_player_side)
    min_score = board_state.get_score_for_side(not max_player_side)

    max_stones = board_state.total_stones_on_side(max_player_side)
    min_stones = board_state.total_stones_on_side(not max_player_side)

    heuristic_val = (max_score - min_score) + (max_stones - min_stones)

    return heuristic_val

def min_max(input_board_state, is_first_move, is_second_move, depth, max_player_side, is_max_player, alpha, beta):
    if is_max_player:
        board_side = max_player_side
    else:
        board_side = not max_player_side

    legal_choices = get_legal_moves(input_board_state, board_side, is_second_move)

    if len(legal_choices) == 0:
        stones_opp_side = input_board_state.total_stones_on_side(not board_side)

        oppScore = stones_opp_side + input_board_state.get_score_for_side(not board_side)

        if oppScore > input_board_state.get_score_for_side(board_side):
            if is_max_player:
                return float("-inf")
            return float("inf")
        elif oppScore < input_board_state.get_score_for_side(board_side):
            if is_max_player:
                float("inf")
            return float("-inf")
        else:
            return 0

    if depth == DEPTH_LIMIT:
        return get_heuristic_val(input_board_state, max_player_side)

    if is_max_player:
        bestVal = float("-inf")

        for move in legal_choices:
            next_state = input_board_state.get_next_state(move, board_side)

            if input_board_state.do_we_play_again(move, board_side, is_first_move):
                val = min_max(next_state, False, False, depth + 1, max_player_side, is_max_player, alpha, beta)
            elif move != SWAP_MOVE:
                val = min_max(next_state, False, is_first_move, depth + 1, max_player_side, not is_max_player, alpha, beta)
            else:
                val = min_max(next_state, False, False, depth + 1, not max_player_side, not is_max_player, alpha, beta)

            if val > bestVal:
                bestVal = val
            if bestVal > alpha:
                alpha = bestVal

            if beta <= alpha:
                break

        return bestVal

    else:
        bestVal = float("inf")

        for move in legal_choices:
            next_state = input_board_state.get_next_state(move, board_side)

            if input_board_state.do_we_play_again(move, board_side, is_first_move):
                val = min_max(next_state, False, False, depth + 1, max_player_side, is_max_player, alpha, beta)
            elif move != SWAP_MOVE:
                val = min_max(next_state, False, is_first_move, depth + 1, max_player_side, not is_max_player, alpha, beta)
            else:
                val = min_max(next_state, False, False, depth + 1, not max_player_side, not is_max_player, alpha, beta)

            if val < bestVal:
                bestVal = val
            if bestVal < beta:
                beta = bestVal

            if beta <= alpha:
                break

        return bestVal
