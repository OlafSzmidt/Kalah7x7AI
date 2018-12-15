//
// Created by laptop on 08/12/2018.
//

#ifndef UNTITLED1_BOARD_H
#define UNTITLED1_BOARD_H

#include <array>

enum Side {NORTH, SOUTH};

Side opposideSide(Side s) {
    switch (s)
    {
        case NORTH: return SOUTH;
        case SOUTH: return NORTH;
        default: return NORTH;  // dummy
    }
}


struct Move {
    Side side;
    int hole;
    bool swapMove;

    Move(Side s, int hole) : side(s), hole(hole), swapMove(false)
    {}

    Move(Side s, int hole, bool swap) : side(s), hole(hole), swapMove(swap)
    {}

    Move(const Move& other) : side(other.side), hole(other.hole), swapMove(other.swapMove)
    {}

    Move(): hole(-1)
    {}

    static Move make_swapMove() {
        return Move(Side(NORTH), -1, true);
    }

    void operator=(const Move& other) {
        side = other.side;
        hole = other.hole;
        swapMove = other.swapMove;
    }

    int getHole() {
        return hole;
    }

    Side getSide() {
        return side;
    }
};

struct Board {

    static const uint32_t NORTH_ROW = 0;
    static const uint32_t SOUTH_ROW = 1;

    static const uint32_t SEEDS_PER_HOLES = 7;

    std::array<std::array<uint32_t, 8>, 2> board;

    static const uint32_t holes = 7;

    static int indexOfSide (Side side)
    {
        switch (side)
        {
            case NORTH: return NORTH_ROW;
            case SOUTH: return SOUTH_ROW;
            default: return -1;  // should never get here
        }
    }

    Board ()
    {
        for (int i=1; i <= holes; i++)
        {
            board[NORTH_ROW][i] = SEEDS_PER_HOLES;
            board[SOUTH_ROW][i] = SEEDS_PER_HOLES;
        }

        board[NORTH_ROW][0] = 0;
        board[SOUTH_ROW][0] = 0;
    }

    Board (const Board& original)
    {
        for (int i=0; i <= holes; i++)
        {
            board[NORTH_ROW][i] = original.board[NORTH_ROW][i];
            board[SOUTH_ROW][i] = original.board[SOUTH_ROW][i];
        }
    }

    Board clone() const
    {
        return Board(*this);
    }

    int getNoOfHoles (  ) const
    {
        return holes;
    }


    int getSeeds (Side side, int hole) const
    {
        if (hole < 1 || hole > holes) {
            throw "Invalid hole number";
        }

        return board[indexOfSide(side)][hole];
    }


    void setSeeds (Side side, int hole, int seeds)
    {
        if (hole < 1 || hole > holes)
            throw "Hole number must be between 1 and 7";
        if (seeds < 0)
            throw "There has to be a non-negative number of seeds";

        board[indexOfSide(side)][hole] = seeds;
    }


    void addSeeds (Side side, int hole, int seeds)
    {
        if (hole < 1 || hole > holes)
            throw "Hole number must be between 1 and 7";
        if (seeds < 0)
            throw "There has to be a non-negative number of seeds";

        board[indexOfSide(side)][hole] += seeds;
    }


    int getSeedsOp (Side side, int hole) const
    {
        if (hole < 1 || hole > holes)
            throw "Hole number must be between 1 and 7";

        return board[1-indexOfSide(side)][holes+1-hole];
    }

    void setSeedsOp (Side side, int hole, int seeds)
    {
        if (hole < 1 || hole > holes)
            throw "Hole number must be between 1 and 7";
        if (seeds < 0)
            throw "There has to be a non-negative number of seeds";

        board[1-indexOfSide(side)][holes+1-hole] = seeds;
    }

    void addSeedsOp (Side side, int hole, int seeds)
    {
        if (hole < 1 || hole > holes)
            throw "Hole number must be between 1 and 7";
        if (seeds < 0)
            throw "There has to be a non-negative number of seeds";

        board[1-indexOfSide(side)][holes+1-hole] += seeds;
    }


    int getSeedsInStore (Side side) const
    {
        return board[indexOfSide(side)][0];
    }


    void setSeedsInStore (Side side, int seeds)
    {
        if (seeds < 0)
            throw "There has to be a non-negative number of seeds";

        board[indexOfSide(side)][0] = seeds;
    }

    void addSeedsToStore (Side side, int seeds)
    {
        if (seeds < 0)
            throw "There has to be a non-negative number of seeds";

        board[indexOfSide(side)][0] += seeds;
    }


};

bool holesEmpty (const Board& board, Side side)
{
    for (int hole = 1; hole <= board.getNoOfHoles(); hole++)
        if (board.getSeeds(side, hole) != 0)
            return false;
    return true;
}

Side makeMove (Board& board, Move move)
{
    int seedsToSow = board.getSeeds(move.getSide(), move.getHole());
    board.setSeeds(move.getSide(), move.getHole(), 0);

    int holes = board.getNoOfHoles();
    int receivingPits = 2*holes + 1;  // sow into: all holes + 1 store
    int rounds = seedsToSow / receivingPits;  // sowing rounds
    int extra = seedsToSow % receivingPits;  // seeds for the last partial round

    if (rounds != 0)
    {
        for (int hole = 1; hole <= holes; hole++)
        {
            board.addSeeds(Side(NORTH), hole, rounds);
            board.addSeeds(Side(SOUTH), hole, rounds);
        }
        board.addSeedsToStore(move.getSide(), rounds);
    }

    // sow the extra seeds (last round):
    Side sowSide = move.getSide();
    int sowHole = move.getHole();  // 0 means store
    for (; extra > 0; extra--){
        // go to next pit:
        sowHole++;
        if (sowHole == 1)  // last pit was a store
            sowSide = opposideSide(sowSide);
        if (sowHole > holes)
        {
            if (sowSide == move.getSide())
            {
                sowHole = 0;  // sow to the store now
                board.addSeedsToStore(sowSide, 1);
                continue;
            }
            else
            {
                sowSide = opposideSide(sowSide);
                sowHole = 1;
            }
        }
        // sow to hole:
        board.addSeeds(sowSide, sowHole, 1);
    }

    // capture:
    if ( (sowSide == move.getSide())  // last seed was sown on the moving player's side ...
         && (sowHole > 0)  // ... not into the store ...
         && (board.getSeeds(sowSide, sowHole) == 1)  // ... but into an empty hole (so now there's 1 seed) ...
         && (board.getSeedsOp(sowSide, sowHole) > 0) )  // ... and the opposite hole is non-empty
    {
        board.addSeedsToStore(move.getSide(), 1 + board.getSeedsOp(move.getSide(), sowHole));
        board.setSeeds(move.getSide(), sowHole, 0);
        board.setSeedsOp(move.getSide(), sowHole, 0);
    }

    // game over?
    // Initialise to NORTH to get rid of uninitialised warning
    Side finishedSide = NORTH;
    bool gameOver = false;

    if (holesEmpty(board, move.getSide())) {
        finishedSide = move.getSide();
        gameOver = true;
    }
    else if (holesEmpty(board, opposideSide(move.getSide()))) {
        finishedSide = opposideSide(move.getSide());
        gameOver = true;
    }
    /* note: it is possible that both sides are finished, but then
       there are no seeds to collect anyway */
    if (gameOver)
    {
        // collect the remaining seeds:
        int seeds = 0;
        Side collectingSide = opposideSide(finishedSide);
        for (int hole = 1; hole <= holes; hole++)
        {
            seeds += board.getSeeds(collectingSide, hole);
            board.setSeeds(collectingSide, hole, 0);
        }
        board.addSeedsToStore(collectingSide, seeds);
    }


    // who's turn is it?
    if (sowHole == 0)  // the store (implies (sowSide == move.getSide()))
        return move.getSide();  // move again
    else
        return opposideSide(move.getSide());
}


#endif //UNTITLED1_BOARD_H
