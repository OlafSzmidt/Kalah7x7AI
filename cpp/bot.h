
#ifndef UNTITLED1_BOT_H
#define UNTITLED1_BOT_H

#include "board.h"
#include <string>
#include "protocol.h"
#include "bot_functions.h"
#include <future>
#include <utility>

struct Bot {
Side mySide;
Board boardState;
bool madeMoveYet = false;
static const int heuristicNumber = 1;
static const bool careAboutReplays = false;

Bot() {}





void runBot() {
    std::string input;
    std::getline(std::cin, input);
    input += "\n";

    MsgType msgType = getMessageType(input);
    
    switch (msgType) {
                case START: {
                    bool amISouth = interpretStartMsg(input);
	   	   
                    if (amISouth) {
                        mySide = Side(SOUTH);
                        doMove(true, false);
                    } else {
                        mySide = Side(NORTH);
                    }

                    break;
                }


                case STATE:
                {
                    MoveTurn moveTurn(interpretStateMsg(input, boardState));

                    if (moveTurn.move == -1) {
                        mySide = opposideSide(mySide);
                    }

                    if (moveTurn.again) {
                        doMove(false, !madeMoveYet);
                    }



                    break;
                }
                case END:
                    exit(1);
            }

}

struct PlayResult {
    Move startMove;
    std::future<int> value;

    PlayResult(const Move& m, std::future<int>&& val) : startMove(m), value(std::forward<std::future<int>>(val))
    {}
};

struct LocalPlayResult {
    Move startMove;
    std::function<int()> task;

    LocalPlayResult(const Move& m, const std::function<int()>& f) : startMove(m), task(f)
    {}
};



void doMove(bool isFirstMove, bool isSecondMove) {
	    madeMoveYet = true;

            std::array<Move, 7> legalMoves = getLegalMoves(boardState, mySide);


            Move bestMove = *std::find_if(legalMoves.begin(), legalMoves.end(), isMoveLegal);
            int bestMoveValue = std::numeric_limits<int>::lowest();
            int alpha = std::numeric_limits<int>::lowest();
            int beta = std::numeric_limits<int>::max();

            // Simulate swap move
            std::vector<PlayResult> tasks;

            int alphCopy = alpha;
            int betaCopy = beta;
            // Simulate swap move
            if (isSecondMove) {
                tasks.emplace_back(Move::make_swapMove(), std::async(iterative_deepening<>, Node(boardState, false, mySide), DEPTH));
            }

            std::vector<LocalPlayResult> localPlayResult;
            bool capturedLocalTask = false;

            for (auto i = legalMoves.begin(); i != legalMoves.end(); ++i) {
                Move& m = *i;


                if (m.hole != -1) {
                    Board nextBoardState(boardState);

                    Side nextTurn = makeMove(nextBoardState, m);

                    if (nextTurn == mySide && !isFirstMove) {
                        if (!capturedLocalTask) {
                            capturedLocalTask = true;
                            auto localTask = [nextBoardState, this]() {return iterative_deepening<>(Node(nextBoardState, true, mySide), DEPTH);};
                            localPlayResult.emplace_back(m, localTask);
                        }
                        else {
                            tasks.emplace_back(m , std::async(iterative_deepening<>, Node(nextBoardState, true, mySide), DEPTH));
                        }
                    }else {
                        if (!capturedLocalTask) {
                            capturedLocalTask = true;
                            auto localTask = [nextBoardState, this]() {return iterative_deepening<>(Node(nextBoardState, false, opposideSide(mySide)),DEPTH);};
                            localPlayResult.emplace_back(m, localTask);

                        }
                        else {
                            tasks.emplace_back(m , std::async(iterative_deepening<>, Node(nextBoardState, false, opposideSide(mySide)), DEPTH));
                        }
                    }
	           
                }
            }


            bestMoveValue = localPlayResult.front().task();
            bestMove = localPlayResult.front().startMove;

            for (auto i = tasks.begin(); i != tasks.end(); ++i) {
                int value = i->value.get();

                if (value > bestMoveValue) {
                    bestMoveValue = value;
                    bestMove = i->startMove;
                }
            }


            std::string moveMsg;
            if (bestMove.swapMove) {
                moveMsg = createSwapMsg();
                mySide = opposideSide(mySide);
            }
            else {
                moveMsg = createMoveMsg(bestMove.getHole());
            }

            std::cout << moveMsg;
    }


};


#endif //UNTITLED1_BOT_H
