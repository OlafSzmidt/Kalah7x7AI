//
// Created by laptop on 09/12/2018.
//

#ifndef UNTITLED1_BOT_FUNCTIONS_H
#define UNTITLED1_BOT_FUNCTIONS_H

#include "board.h"
#include <vector>
#include <algorithm>
#include <limits>

const int DEPTH = 16;

int getSeedsOnBoardSide(const Board& b, Side s);

int heuristicValue(const Board& b, Side maxPlayerSide) {
    int maxScore = b.getSeedsInStore(maxPlayerSide);
    int minScore = b.getSeedsInStore(opposideSide(maxPlayerSide));

    int maxSeeds = getSeedsOnBoardSide(b, maxPlayerSide);
    int minSeeds = getSeedsOnBoardSide(b, opposideSide(maxPlayerSide));

    int stonesInFirstTwo = b.getSeeds(maxPlayerSide, 1) + b.getSeeds(maxPlayerSide, 2);
    int stonesInLastTwo = b.getSeeds(maxPlayerSide, 6) + b.getSeeds(maxPlayerSide, 7);

    int value = (maxScore - minScore)  + (maxSeeds - minSeeds);

    return value;
}

std::array<Move, 7> getLegalMoves(const Board& b, Side s) {
    std::array<Move, 7> legalMoves;

    for (int i = 1; i <= b.getNoOfHoles(); ++i) {
        int seedsInHole = b.getSeeds(s, i);

        if (seedsInHole > 0) {
            legalMoves.at(i - 1) = Move(s, i);
        }
    }

    return legalMoves;
}

int getSeedsOnBoardSide(const Board& b, Side s) {
    int total = 0;

    for (int i = 1; i <= b.getNoOfHoles(); ++i) {
        total += b.getSeeds(s, i);
    }

    return total;
}

bool isMoveIllegal(const Move& m) {
    return m.hole == -1;
}

bool isMoveLegal(const Move& m) {
    return m.hole != -1;
}

int minMax(const Board& b, Side maxPlayerSide, const bool isMaxPlayer, int alpha, int beta, const bool isSecondMove, const int depth) {
    Side board_side;

    if (!isMaxPlayer) {
        board_side = opposideSide(maxPlayerSide);
    }
    else {
        board_side = maxPlayerSide;
    }

    std::array<Move, 7> legalMoves = getLegalMoves(b, board_side);

    if (std::all_of(legalMoves.begin(), legalMoves.end(), isMoveIllegal)) {
        int maxScore = b.getSeedsInStore(maxPlayerSide) + getSeedsOnBoardSide(b, maxPlayerSide);
        int minScore = b.getSeedsInStore(opposideSide(maxPlayerSide)) + getSeedsOnBoardSide(b, opposideSide(maxPlayerSide));

        int value = maxScore > minScore ? std::numeric_limits<int>::max() : maxScore == minScore ? 0 : std::numeric_limits<int>::lowest();

        return value;
    }

    if (depth == DEPTH) {
        int value = heuristicValue(b, maxPlayerSide);
        return heuristicValue(b, maxPlayerSide);
    }

    if (isMaxPlayer) {
        int bestValue = std::numeric_limits<int>::lowest();

        if (isSecondMove) {
            // Then swap is available

            int value = minMax(b, opposideSide(maxPlayerSide), !isMaxPlayer, alpha, beta, false, depth + 1);

            if (value > bestValue) {
                bestValue = value;
            }

            if (bestValue > alpha) {
                alpha = bestValue;
            }

            if (beta <= alpha) {
                return bestValue;
            }
        }

        for (Move m : legalMoves) {
            if (m.hole != -1) {
                Board nextBoardState(b);
                Side nextTurn = makeMove(nextBoardState, m);

                int value;
                if (nextTurn == board_side) {
                    // play again
                    value = minMax(nextBoardState, maxPlayerSide, isMaxPlayer, alpha, beta, false, depth + 1);
                } else {
                    value = minMax(nextBoardState, maxPlayerSide, !isMaxPlayer, alpha, beta, false, depth + 1);
                }

                if (value > bestValue) {
                    bestValue = value;
                }

                if (bestValue > alpha) {
                    alpha = bestValue;
                }

                if (beta <= alpha) {
                    break;
                }
            }

        }

        return bestValue;
    }
    else {
        int bestValue = std::numeric_limits<int>::max();

        if (isSecondMove) {
            // Then swap is available

            int value = minMax(b, opposideSide(maxPlayerSide), !isMaxPlayer, alpha, beta, false, depth + 1);

            if (value < bestValue) {
                bestValue = value;
            }

            if (bestValue < beta) {
                beta = bestValue;
            }

            if (beta <= alpha) {
                return bestValue;
            }
        }

        for (Move m : legalMoves) {
            if (m.hole != -1) {
                Board nextBoardState(b);
                Side nextTurn = makeMove(nextBoardState, m);

                int value;
                if (nextTurn == board_side) {
                    // play again
                    value = minMax(nextBoardState, maxPlayerSide, isMaxPlayer, alpha, beta, false, depth + 1) ;

                } else {
                    value = minMax(nextBoardState, maxPlayerSide, !isMaxPlayer, alpha, beta, false, depth + 1);
                }

                if (value < bestValue) {
                    bestValue = value;
                }

                if (bestValue < beta) {
                    beta = bestValue;
                }

                if (beta <= alpha) {
                    break;
                }
            }
        }

        return bestValue;
    }
}



#endif //UNTITLED1_BOT_FUNCTIONS_H
