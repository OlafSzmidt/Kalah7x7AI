#include <iostream>
#include "bot.h"


template <typename H1, typename H2>
bool playGame(Bot<H1>& b, Bot<H2>& b2) {
    Board globalBoard;

    Side bSide(SOUTH);
    Side b2Side(NORTH);

    Move firstMove = b.onStart(true);
    b2.onStart(false);

    makeMove(globalBoard, firstMove);
    b.boardState = globalBoard;
    b2.boardState = globalBoard;

    Move secondMove = b2.doMove(false, true);

    if (secondMove.swapMove) {
        bSide = opposideSide(bSide);
        b2Side = opposideSide(b2Side);

        b.mySide = bSide;
        b2.mySide = b2Side;
    }
    else {
        Side playSide = makeMove(globalBoard, secondMove);
        b.boardState = globalBoard;
        b2.boardState = globalBoard;


        while (true) {
            Bot& chosenBot = playSide == bSide ? b: b2;

            Move move = chosenBot.doMove(false, false);
            Side nextTurnSide = makeMove(globalBoard, move);
            b.boardState = globalBoard;
            b2.boardState = globalBoard;

            playSide = nextTurnSide;

            if (!hasLegalMoves(globalBoard, playSide)) {
                break;
            }
        }

        int bScore = globalBoard.getSeedsInStore(bSide) + getSeedsOnBoardSide(globalBoard, bSide);
        int b2Score = globalBoard.getSeedsInStore(b2Side) + getSeedsOnBoardSide(globalBoard, b2Side);

        bool winner = bScore > b2Score ? true : false;

        return winner;
    }
}

int main() {

    try{

        Bot<DefaultHeuristic> b;
      for (;;) {
        b.runBot();
      }
    } catch(const std::system_error& e) {
        std::cout << "Caught system_error with code " << e.code() 
                  << " meaning " << e.what() << '\n';
    }
}
