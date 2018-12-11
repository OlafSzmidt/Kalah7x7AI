
#ifndef UNTITLED1_BOT_H
#define UNTITLED1_BOT_H

#include "board.h"
#include <string>
#include "protocol.h"
#include "bot_functions.h"
#include <future>
#include <utility>

template <typename H = DefaultHeuristic>
struct Bot {
Side mySide;
Board boardState;
bool madeMoveYet = false;
static const int heuristicNumber = 1;
static const bool careAboutReplays = false;


Move onStart(bool amISouth) {
    if (amISouth) {
        mySide = Side(SOUTH);
        return doMove(true, false);
    } else {
        mySide = Side(NORTH);
    }
}

void onState(const MoveTurn& moveTurn) {
    if (moveTurn.again) {
        doMove(false, !madeMoveYet);
    }

    if (moveTurn.move == -1) {
        mySide = opposideSide(mySide);
    }
}


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

                    if (moveTurn.again) {
                        doMove(false, !madeMoveYet);
                    }

                    if (moveTurn.move == -1) {
                        mySide = opposideSide(mySide);
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


Move doMove(bool isFirstMove, bool isSecondMove) {
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
                tasks.emplace_back(Move::make_swapMove(), std::async(minMax<H>, boardState, opposideSide(mySide), false, alphCopy, betaCopy, false, 1));
            }

            for (auto i = legalMoves.begin(); i != legalMoves.end(); ++i) {
                Move& m = *i;

                if (m.hole != -1) {
                    Board nextBoardState(boardState);

                    Side nextTurn = makeMove(nextBoardState, m);
		   
                    if (nextTurn == mySide && !isFirstMove) {
                        tasks.emplace_back(m , std::async(minMax<H>, nextBoardState, mySide, true, alphCopy, betaCopy, false, 1));
                    }else {
                        tasks.emplace_back(m , std::async(minMax<H>, nextBoardState, mySide, false, alphCopy, betaCopy, isFirstMove, 1));
                    }
	           
                }
            }
	    
            
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

            return bestMove;
    }


};


#endif //UNTITLED1_BOT_H
