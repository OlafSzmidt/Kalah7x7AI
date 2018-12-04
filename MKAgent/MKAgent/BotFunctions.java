package MKAgent;

import java.util.ArrayList;
import java.util.List;

public class BotFunctions {
    private static final int DEPTH = 13;

    public static List<Move> getLegalMoves(Board b, Side s) {
        List<Move> legalMoves = new ArrayList<>();

        for (int i = 1; i <= b.getNoOfHoles(); ++i) {
            int seedsInHole = b.getSeeds(s, i);

            if (seedsInHole > 0) {
                legalMoves.add(new Move(s, i));
            }
        }

        return legalMoves;
    }

    static int getSeedsOnBoardSide(Board b, Side s) {
        int total = 0;

        for (int i = 1; i <= b.getNoOfHoles(); ++i) {
            total += b.getSeeds(s, i);
        }

        return total;
    }

    public static int heuristicValue(Board b, Side maxPlayerSide) {
        int maxScore = b.getSeedsInStore(maxPlayerSide);
        int minScore = b.getSeedsInStore(maxPlayerSide.opposite());

        int maxSeeds = getSeedsOnBoardSide(b, maxPlayerSide);
        int minSeeds = getSeedsOnBoardSide(b, maxPlayerSide.opposite());

        return (maxScore - minScore) + (maxSeeds - minSeeds);
    }

    public static int minMax(Board b, Side maxPlayerSide, boolean isMaxPlayer, int alpha, int beta, boolean isSecondMove, int depth) {
        Side board_side;

        if (!isMaxPlayer) {
            board_side = maxPlayerSide.opposite();
        }
        else {
            board_side = maxPlayerSide;
        }

        List<Move> legalMoves = getLegalMoves(b, board_side);

        if (legalMoves.isEmpty()) {
            int maxScore = b.getSeedsInStore(maxPlayerSide) + getSeedsOnBoardSide(b, maxPlayerSide);
            int minScore = b.getSeedsInStore(maxPlayerSide.opposite()) + getSeedsOnBoardSide(b, maxPlayerSide.opposite());

            return maxScore > minScore ? Integer.MAX_VALUE : maxScore == minScore ? 0 : Integer.MIN_VALUE;
        }

        if (depth == DEPTH) {
            return heuristicValue(b, maxPlayerSide);
        }

        if (isMaxPlayer) {
            int bestValue = Integer.MIN_VALUE;

            if (isSecondMove) {
                // Then swap is available

                int value = minMax(b, maxPlayerSide.opposite(), !isMaxPlayer, alpha, beta, false, depth + 1);

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
                Board nextBoardState = new Board(b);
                Side nextTurn = new Kalah(nextBoardState).makeMove(m);

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

            return bestValue;


        }
        else {
            int bestValue = Integer.MAX_VALUE;

            if (isSecondMove) {
                // Then swap is available

                int value = minMax(b, maxPlayerSide.opposite(), !isMaxPlayer, alpha, beta, false, depth + 1);

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
                Board nextBoardState = new Board(b);
                Side nextTurn = new Kalah(nextBoardState).makeMove(m);

                int value;
                if (nextTurn == board_side) {
                    // play again
                    value = minMax(nextBoardState, maxPlayerSide, isMaxPlayer, alpha, beta, false, depth + 1);
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

            return bestValue;
        }
    }
}
