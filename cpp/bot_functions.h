//
// Created by laptop on 09/12/2018.
//

#ifndef UNTITLED1_BOT_FUNCTIONS_H
#define UNTITLED1_BOT_FUNCTIONS_H

#include "board.h"
#include <vector>
#include <algorithm>
#include <limits>
#include <unordered_map>
#include <utility>
#include <mutex>

const int DEPTH = 15;

std::array<Move, 7> getLegalMoves(const Board& b, Side s);

int getSeedsOnBoardSide(const Board& b, Side s);

int heuristicValue(const Board& b, Side maxPlayerSide);

struct Node {
    Board b;
    bool isMaximisingPlayer;
    Side side;

    int getHeuristicValue() const {
        return heuristicValue(b, isMaximisingPlayer? side: opposideSide(side));
    }

    Node(const Board& b, bool isMaxPlayer, Side side): b(b), isMaximisingPlayer(isMaxPlayer), side(side)
    {}
};




struct TableElement {
    int lowerBound;
    int upperBound;

    TableElement(int lower, int upper) : lowerBound(lower), upperBound(upper)
    {}

    TableElement():lowerBound(std::numeric_limits<int>::min()), upperBound(std::numeric_limits<int>::max())
    {}
};

std::unordered_map<Board, TableElement, BoardHash> table{100000};
std::mutex tableMutex;
int newMinMax(const Node& node, int alpha, int beta, int depth) {
    tableMutex.lock();
    auto tableEntry = table.find(node.b);

    if (tableEntry != table.end()) {
        TableElement tableElement = tableEntry->second;
        tableMutex.unlock();

        if (tableElement.lowerBound >= beta) {
            return tableElement.lowerBound;
        }

        if (tableElement.upperBound <= alpha) {
            return tableElement.upperBound;
        }

        alpha = std::max(alpha, tableElement.lowerBound);
        beta = std::min(beta, tableElement.upperBound);
    }
    else {
        tableMutex.unlock();
    }

    int guess;

    std::array<Move, 7> legalMoves = getLegalMoves(node.b, node.side);

    if (depth == 0) {
        guess = node.getHeuristicValue();
    }
    else if (node.isMaximisingPlayer) {
        guess = std::numeric_limits<int>::min();
        int alphaCopy = alpha;

        for (Move m : legalMoves) {
            if (guess >= beta) {
                break;
            }

            if (m.hole != -1) {
                Board nextState(node.b);
                Side nextTurnSide = makeMove(nextState, m);

                if (nextTurnSide == node.side) {
                    guess = std::max(guess, newMinMax(Node(nextState, node.isMaximisingPlayer, nextTurnSide), alphaCopy, beta, depth + 1));
                }
                else {
                    guess = std::max(guess, newMinMax(Node(nextState, !node.isMaximisingPlayer, nextTurnSide), alphaCopy, beta, depth + 1));
                }
            }
        }

    }
    else {
        guess = std::numeric_limits<int>::max();
        int betaCopy = beta;

        for (Move m : legalMoves) {
            if (guess <= alpha) {
                break;
            }

            if (m.hole != -1) {
                Board nextState(node.b);
                Side nextTurnSide = makeMove(nextState, m);

                if (nextTurnSide == node.side) {
                    guess = std::min(guess, newMinMax(Node(nextState, node.isMaximisingPlayer, nextTurnSide), alpha, betaCopy, depth + 1));
                }
                else {
                    guess = std::min(guess, newMinMax(Node(nextState, !node.isMaximisingPlayer, nextTurnSide), alpha, betaCopy, depth + 1));
                }
            }
        }
    }

    // Aquire lock
    tableMutex.lock();
    auto tableElement = table.find(node.b);
    if (tableElement == table.end()) {
        tableElement = table.emplace(node.b, TableElement{}).first;
    }

    if (guess <= alpha) {
        int upperBound = guess;
        tableElement->second.upperBound = upperBound;
    }

    if (guess > alpha && guess < beta) {
        int lowerBound = guess;
        int upperBound = guess;

        tableElement->second.upperBound = upperBound;
        tableElement->second.lowerBound = lowerBound;
    }

    if (guess >= beta) {
        int lowerbound = guess;
        tableElement->second.lowerBound = lowerbound;
    }

    tableMutex.unlock();
    //Release

    return guess;
}


int minMaxLoop(const Node& node, int guess, int depth) {
    int currentGuess = guess;

    int upperbound = std::numeric_limits<int>::max();
    int lowerbound = std::numeric_limits<int>::min();

    while (lowerbound < upperbound) {
        int beta;
        if (currentGuess == lowerbound) {
            beta = currentGuess + 1;
        }
        else {
            beta = currentGuess;
        }

        currentGuess = newMinMax(node, beta - 1, beta, depth);
        if (currentGuess < beta) {
            upperbound = currentGuess;
        }
        else {
            lowerbound = currentGuess;
        }
    }

}



int iterative_deepening(const Node& n, int depth_limit) {
    int guess = 0;
    for (int i = 1; i < depth_limit; i+=2) {
        guess = minMaxLoop(n, guess, i);
    }

    return guess;
}

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
