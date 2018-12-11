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
    static array<array<int, 3>, 4> weights;

    static int call(const Board& b, Side maxPlayerSide) {
        int maxScore = b.getSeedsInStore(maxPlayerSide);
        int minScore = b.getSeedsInStore(opposideSide(maxPlayerSide));

        int maxSeeds = getSeedsOnBoardSide(b, maxPlayerSide);
        int minSeeds = getSeedsOnBoardSide(b, opposideSide(maxPlayerSide));

        int stonesInFirstTwo = b.getSeeds(maxPlayerSide, 1) + b.getSeeds(maxPlayerSide, 2);
        int stonesInLastTwo = b.getSeeds(maxPlayerSide, 6) + b.getSeeds(maxPlayerSide, 7);

        array<array<int, 3>, 3> featureVectors;

        for (int i = 0; i < featureVectors.size(); ++i) {
            auto& featureVector = featureVectors[i];
            featureVector[0] = maxScore - minScore;
            featureVector[1] = maxSeeds - minSeeds;
            featureVector[2] = stonesInFirstTwo - stonesInLastTwo;
        }

        array<int, 3> outputFeatures;

        for (int i = 0; i < weights.size() - 1; ++i) {
            outputFeatures[i] = dotProduct<3>(featureVectors[i], weights[i]);
        }

        return dotProduct<3>(outputFeatures, weights[weights.size() - 1]);
    }
};


struct Heuristic2 {
    static array<array<int, 3>, 4> weights;

    static int call(const Board& b, Side maxPlayerSide) {
        int maxScore = b.getSeedsInStore(maxPlayerSide);
        int minScore = b.getSeedsInStore(opposideSide(maxPlayerSide));

        int maxSeeds = getSeedsOnBoardSide(b, maxPlayerSide);
        int minSeeds = getSeedsOnBoardSide(b, opposideSide(maxPlayerSide));

        int stonesInFirstTwo = b.getSeeds(maxPlayerSide, 1) + b.getSeeds(maxPlayerSide, 2);
        int stonesInLastTwo = b.getSeeds(maxPlayerSide, 6) + b.getSeeds(maxPlayerSide, 7);

        array<array<int, 3>, 3> featureVectors;

        for (int i = 0; i < featureVectors.size(); ++i) {
            auto& featureVector = featureVectors[i];
            featureVector[0] = maxScore - minScore;
            featureVector[1] = maxSeeds - minSeeds;
            featureVector[2] = stonesInFirstTwo - stonesInLastTwo;
        }

        array<int, 3> outputFeatures;

        for (int i = 0; i < weights.size() - 1; ++i) {
            outputFeatures[i] = dotProduct<3>(featureVectors[i], weights[i]);
        }

        return dotProduct<3>(outputFeatures, weights[weights.size() - 1]);
    }
};

template <typename Winner, typename Loser>
void updateWeights() {
    for (int i = 0; i < Winner::weights.size(); ++i) {
        for (int j = 0; j < Winner::weights[i].size(); ++j) {
            Loser::weights[i][j] = Winner::weights[i][j] + rand() % 40 - 20;
        }
    }
}

template <typename T, int Length>
void printArray(const array<T, Length> array) {
    for (int i = 0; i < Length; ++i) {
        cout << array[i] << ",";
    }

    cout << "\n\n";
}

template <typename T, int Length1, int Length2>
void print2DArray(const array<array<T, Length2>, Length1> twoDArray) {
    for (int i = 0; i < Length1; ++i) {
        for (int j = 0; j < Length2; ++j) {
            cout << twoDArray[i][j] << ", ";
        }

        cout << "\n";
    }

    cout << "\n";
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

array<array<int, 3>, 4> Heuristic1::weights = {{{1, 1, 1},
                                               {1, 1, 1},
                                               {1, 1, 1},
                                               {1,1, 1}}};

array<array<int, 3>, 4> Heuristic2::weights = {{{1, 1, 1},
                                               {1, 1, 1},
                                               {1, 1, 1},
                                               {1,1, 1}}};

int main() {


    Bot<Heuristic1> b1;
    Bot<Heuristic2> b2;

    while (true) {
        bool b1Wins = playGame(b1, b2);

        if (b1Wins) {
            updateWeights<Heuristic1, Heuristic2>();
            print2DArray<int, 4, 3>(Heuristic1::weights);
        }
        else {
            updateWeights<Heuristic2, Heuristic1>();
            print2DArray<int, 4, 3>(Heuristic2::weights);
        }


    }
}
