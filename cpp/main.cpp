#include <iostream>
#include "bot.h"


using namespace std;

array<int, 2> weights1 = {1, 1};
array<int, 2> weights2 = {1, 1};

int heuristic1(const Board& b, Side maxPlayerSide) {
    int maxScore = b.getSeedsInStore(maxPlayerSide);
    int minScore = b.getSeedsInStore(opposideSide(maxPlayerSide));

    int maxSeeds = getSeedsOnBoardSide(b, maxPlayerSide);
    int minSeeds = getSeedsOnBoardSide(b, opposideSide(maxPlayerSide));

    int stonesInFirstTwo = b.getSeeds(maxPlayerSide, 1) + b.getSeeds(maxPlayerSide, 2);
    int stonesInLastTwo = b.getSeeds(maxPlayerSide, 6) + b.getSeeds(maxPlayerSide, 7);

    int value = weights1[0] * (maxScore - minScore) + weights1[1] * (maxSeeds - minSeeds);

    return value;
}

int heuristic2(const Board& b, Side maxPlayerSide) {
    int maxScore = b.getSeedsInStore(maxPlayerSide);
    int minScore = b.getSeedsInStore(opposideSide(maxPlayerSide));

    int maxSeeds = getSeedsOnBoardSide(b, maxPlayerSide);
    int minSeeds = getSeedsOnBoardSide(b, opposideSide(maxPlayerSide));

    int stonesInFirstTwo = b.getSeeds(maxPlayerSide, 1) + b.getSeeds(maxPlayerSide, 2);
    int stonesInLastTwo = b.getSeeds(maxPlayerSide, 6) + b.getSeeds(maxPlayerSide, 7);

    int value = weights2[0] * (maxScore - minScore) + weights2[1] * (maxSeeds - minSeeds);

    return value;
}




bool playGame(Bot& b, Bot& b2) {
    Board globalBoard;

    Side bSide(SOUTH);
    Side b2Side(NORTH);

    Move firstMove = b.onStart(true);
    b2.onStart(false);

    makeMove(globalBoard, firstMove);
    b.boardState = globalBoard;
    b2.boardState = globalBoard;

    Move secondMove = b2.doMove(false, true);

    Side playSide;
    if (secondMove.swapMove) {
        bSide = opposideSide(bSide);
        b2Side = opposideSide(b2Side);

        b.mySide = bSide;
        b2.mySide = b2Side;

        playSide = bSide;
    }
    else {
        playSide = makeMove(globalBoard, secondMove);
    }


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

    b = Bot(b.heuristic);
    b2  = Bot(b2.heuristic);

    return bScore > b2Score;

}

int main() {
    Bot b1(heuristic1);
    Bot b2(heuristic2);

    while (true) {
        bool b1Wins = playGame(b1, b2);

        if (b1Wins) {
            weights2[0] = weights1[0] + rand() % 10 - 5;
            weights2[1] = weights1[1] + rand() % 10 - 5;

            std::cout << weights1[0] << ", " << weights1[1] << "\n";
        }
        else {
            weights1[0] = weights2[0] + rand() % 10 - 5;
            weights1[1] = weights2[1] + rand() % 10 - 5;

            std::cout << weights2[0] << ", " << weights2[1] << "\n";
        }


    }



}
