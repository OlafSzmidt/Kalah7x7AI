#include <iostream>
#include "bot.h"


using namespace std;

template <int Length>
int dotProduct(array<int, Length> first, array<int, Length> second) {
    int total = 0;

    for (int i = 0; i < Length; ++i) {
        total += first[i] * second[i];
    }

    return total;
}

struct Heuristic1 {
    static array<int, 3> weights;

    static int call(const Board& b, Side maxPlayerSide) {
        int maxScore = b.getSeedsInStore(maxPlayerSide);
        int minScore = b.getSeedsInStore(opposideSide(maxPlayerSide));

        int maxSeeds = getSeedsOnBoardSide(b, maxPlayerSide);
        int minSeeds = getSeedsOnBoardSide(b, opposideSide(maxPlayerSide));

        int stonesInFirstTwo = b.getSeeds(maxPlayerSide, 1) + b.getSeeds(maxPlayerSide, 2);
        int stonesInLastTwo = b.getSeeds(maxPlayerSide, 6) + b.getSeeds(maxPlayerSide, 7);

        array<int, 3> featuresVector;

        featuresVector[0] = maxScore - minScore;
        featuresVector[1] = maxSeeds - minSeeds;
        featuresVector[2] = stonesInFirstTwo - stonesInLastTwo;

        return dotProduct<3>(featuresVector, weights);
    }
};


struct Heuristic2 {
    static array<int, 3> weights;

    static int call(const Board& b, Side maxPlayerSide) {
        int maxScore = b.getSeedsInStore(maxPlayerSide);
        int minScore = b.getSeedsInStore(opposideSide(maxPlayerSide));

        int maxSeeds = getSeedsOnBoardSide(b, maxPlayerSide);
        int minSeeds = getSeedsOnBoardSide(b, opposideSide(maxPlayerSide));

        int stonesInFirstTwo = b.getSeeds(maxPlayerSide, 1) + b.getSeeds(maxPlayerSide, 2);
        int stonesInLastTwo = b.getSeeds(maxPlayerSide, 6) + b.getSeeds(maxPlayerSide, 7);

        array<int, 3> featuresVector;

        featuresVector[0] = maxScore - minScore;
        featuresVector[1] = maxSeeds - minSeeds;
        featuresVector[2] = stonesInFirstTwo - stonesInLastTwo;

        return dotProduct<3>(featuresVector, weights);
    }
};

template <typename Winner, typename Loser>
void updateWeights() {
    for (int i = 0; i < Winner::weights.size(); ++i) {
        Loser::weights[i] = Winner::weights[i] + rand() % 40 - 20;
    }
}

template <typename T, int Length>
void printArray(const array<T, Length> array) {
    for (int i = 0; i < Length; ++i) {
        cout << array[i] << ",";
    }

    cout << "\n\n";
}

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
        Move move;
        if (playSide == bSide) {
            move = b.doMove(false, false);
        }
        else {
            move = b2.doMove(false, false);
        }

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

    b = Bot<H1>();
    b2  = Bot<H2>();

    return bScore > b2Score;

}

array<int, 3> Heuristic1::weights = {1, 1, 1};
array<int, 3> Heuristic2::weights = {1, 1, 1};

int main() {


    Bot<Heuristic1> b1;
    Bot<Heuristic2> b2;

    while (true) {
        bool b1Wins = playGame(b1, b2);

        if (b1Wins) {
            updateWeights<Heuristic1, Heuristic2>();
            printArray<int, 3>(Heuristic1::weights);
        }
        else {
            updateWeights<Heuristic2, Heuristic1>();
            printArray<int, 3>(Heuristic2::weights);
        }


    }
}
