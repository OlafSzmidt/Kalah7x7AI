//
// Created by laptop on 09/12/2018.
//

#ifndef UNTITLED1_BOT_FUNCTIONS_H
#define UNTITLED1_BOT_FUNCTIONS_H

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <random>
#include <vector>

#include "board.h"

#define MONTE_CARLO 1

#if MONTE_CARLO
double const UCT_BIAS_CONSTANT = 1 / std::sqrt(2);
#endif

const int DEPTH = 14;

int getSeedsOnBoardSide(const Board& b, Side s);

int heuristicValue(const Board& b, Side maxPlayerSide) {
    int maxScore = b.getSeedsInStore(maxPlayerSide);
    int minScore = b.getSeedsInStore(opposideSide(maxPlayerSide));

    int maxSeeds = getSeedsOnBoardSide(b, maxPlayerSide);
    int minSeeds = getSeedsOnBoardSide(b, opposideSide(maxPlayerSide));

    int stonesInFirstTwo = b.getSeeds(maxPlayerSide, 1) + b.getSeeds(maxPlayerSide, 2);
    int stonesInLastTwo = b.getSeeds(maxPlayerSide, 6) + b.getSeeds(maxPlayerSide, 7);

    int value = 32 * (maxScore - minScore) + 32 * (maxSeeds - minSeeds);

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

#if MONTE_CARLO
struct MCNode {
    long payoff;
    long count;
    std::vector<MCNode> children;
    MCNode* parent;
    Board board;
    Side side;
    bool initialised;
    
    MCNode() : payoff(0), count(-1), parent(nullptr) {}
//    MCNode(MCNode* p) : payoff(0), count(0), parent(p) {}
    MCNode(MCNode* p, const Board& b, const Side& s) : payoff(0), count(0), parent(p), board(b), side(s), initialised(false) {}
    
    void initChildren() {
        std::array<Move, 7> legalMoves = getLegalMoves(board, side);
        
        for (auto it = legalMoves.begin(); it < legalMoves.end(); ++it) {
            Move& move = *it;
            
            if (isMoveLegal(move)) {
                Board theBoard(board);
                makeMove(theBoard, move);
                children.emplace_back(this, theBoard, opposideSide(side));
            }
        }
        initialised = true;
    }
    
    bool isTerminalNode() const {
        return initialised && (children.size() == 0);
    }
    
    bool isFullyExpanded() const {
        if (!initialised) {
            return false;
        }
//        else if (children.size() == 0) {
//            return true;
//        }
        auto predicate = [](const MCNode& node) { return node.count > 0; };
        return std::all_of(children.begin(), children.end(), predicate);
    }
    
    double getUCT() const {
        double ratio = 0.0;
        double upperBound = 0.0;
        if (count > 0) {
            ratio = payoff / static_cast<double>(count);
            upperBound = UCT_BIAS_CONSTANT * std::sqrt((2 * std::log(parent->count)) / count);
        }
        
        return ratio + upperBound;
    }
    
    MCNode* getFirstUnvisited() {
        if (!initialised) {
            initChildren();
        }

        MCNode* result = nullptr;
        auto predicate = [](const MCNode& node) { return node.count == 0; };
        auto first = children.begin();
        auto last = children.end();
        
        auto it = std::find_if(first, last, predicate);
        if (it != last) {
            result = &*it;
        }
        
        return result;
    }
};

MCNode& mcBestUCT(MCNode& node) {
    auto comp = [](const MCNode& a, const MCNode& b) {
        return a.getUCT() < b.getUCT();
    };
    
    return *std::max_element(node.children.begin(), node.children.end(), comp);
}

MCNode& mcSelection(MCNode& root) {
    MCNode* node = &root;
    while(node->isFullyExpanded()) {
        if (node->isTerminalNode()) {
            return *node;
        }
        node = &mcBestUCT(*node);
    }
    
    MCNode* result = node->getFirstUnvisited();
    if (result->count == -1) {
        result = node;
    }
    
    return *result;
}

MCNode& simulationGetNextNode(MCNode& node) {
    static std::random_device randomDevice;
    static std::mt19937 generator(randomDevice());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    if (!node.initialised) {
        node.initChildren();
        
        // We might init a terminal node
        if (node.children.size() == 0) {
            return node;
        }
    }
    int childrenSize = static_cast<int>(node.children.size());
    int nextChildIndex = static_cast<int>(dist(generator) * childrenSize) % childrenSize;
    
    return node.children.at(nextChildIndex);
}

int mcSimulation(MCNode& node, Side maxPlayerSide) {
    if (!node.initialised) {
//        std::cout << node.initialised << std::endl;
        node.initChildren();
    }

    MCNode* currentNode = &node;
    while (!currentNode->isTerminalNode()) {
        currentNode = &simulationGetNextNode(*currentNode);
    }
    
    // Collect the remaining seeds:
    Board& board = currentNode->board;
    int seeds = 0;
    Side collectingSide = opposideSide(currentNode->side);
    for (int hole = 1; hole <= board.getNoOfHoles(); hole++)
    {
        seeds += board.getSeeds(collectingSide, hole);
        board.setSeeds(collectingSide, hole, 0);
    }
    board.addSeedsToStore(collectingSide, seeds);
    
    // Calculate payoff
    int payoff = 0;
    int maxPlayerScore = board.getSeedsInStore(maxPlayerSide);
    int minPlayerScore = board.getSeedsInStore(opposideSide(maxPlayerSide));
    if (maxPlayerScore > minPlayerScore) {
        payoff = 1;
    }
    else if (maxPlayerScore < minPlayerScore) {
        payoff = -1;
    } // If equal payoff = 0
    
    return payoff;
}

void mcBackup(MCNode& node, int payoff) {
    node.payoff += payoff;
    ++node.count;
    
    if (!node.parent) {
        return;
    }
    
    mcBackup(*node.parent, payoff);
}

double monteCarlo(const Board& board, Side side, int timeSeconds, Side maxPlayerSide) {
    MCNode root(nullptr, board, side);
    auto startTime = std::chrono::system_clock::now();
    long elapsedTime = 0;
    while (elapsedTime <= timeSeconds) {
        MCNode& leaf = mcSelection(root);
        int simulationResult = mcSimulation(leaf, maxPlayerSide);
        mcBackup(leaf, simulationResult);
        
        auto diff = std::chrono::system_clock::now() - startTime;
        elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
    }

    return root.payoff / static_cast<double>(root.count);
}
#endif

double minMax(const Board& b, Side maxPlayerSide, const bool isMaxPlayer, int alpha, int beta, const bool isSecondMove, const int depth) {
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
#if MONTE_CARLO
        double value = monteCarlo(b, board_side, 5, maxPlayerSide);
#else
        int value = heuristicValue(b, maxPlayerSide);
#endif
        return value;
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
