package MKAgent;

import java.io.IOException;
import java.util.List;
import java.util.Scanner;
import static MKAgent.BotFunctions.minMax;

import java.util.logging.FileHandler;
import java.util.logging.Logger;

public class Bot {
    private Side mySide;
    private Scanner scanner = new Scanner(System.in);
    private Board boardState = new Board(7, 7);
    private boolean madeMoveYet = false;
    private Logger logger = Logger.getLogger(Bot.class.getName());

    public Bot() throws IOException
        {

        }

    public static void main(String[] args) throws Exception {
        Bot b = new Bot();
        for(;;) {
            b.runBot();
        }
    }

    public void runBot() throws Exception {
        String input = scanner.nextLine();
        input += "\n";

        logger.info("Waiting for message");
        MsgType msgType = Protocol.getMessageType(input);

        switch (msgType) {
            case START:
                boolean amISouth = Protocol.interpretStartMsg(input);

                if (amISouth) {
                    mySide = Side.SOUTH;
                    String name = "Player1";
                    logger.addHandler(new FileHandler(name + "_log.txt"));
                    makeMove(true, false);
                }
                else {
                    String name = "Player2";
                    logger.addHandler(new FileHandler(name + "_log.txt"));
                    mySide = Side.NORTH;
                }



                break;


            case STATE:
                Protocol.MoveTurn moveTurn = Protocol.interpretStateMsg(input, boardState);

                if (moveTurn.again) {
                    makeMove(false, !madeMoveYet);
                }

                if (moveTurn.move == -1) {
                    mySide = mySide.opposite();
                }


                break;

            case END:
                System.exit(-1);
                break;
        }


    }

    public void makeMove(boolean isFirstMove, boolean isSecondMove) {
        madeMoveYet = true;

        List<Move> legalMoves = BotFunctions.getLegalMoves(boardState, mySide);

        logger.info("BoardState");
        logger.info(boardState.toString());
        logger.info(legalMoves.toString());

        Move bestMove = new Move(mySide, 1);
        int bestMoveValue = Integer.MIN_VALUE;
        int alpha = Integer.MIN_VALUE;
        int beta = Integer.MAX_VALUE;

        logger.info("Simulating game");
        // Simulate swap move
        if (isSecondMove) {
            int value = minMax(boardState, mySide.opposite(), false, alpha, beta, false, 1);

            if (value > bestMoveValue) {
                bestMoveValue = value;
                bestMove = new Move.SwapMove();
            }

            alpha = bestMoveValue;
        }

        for (Move m : legalMoves) {
            Board nextBoardState = new Board(boardState);
            Side nextTurn = new Kalah(nextBoardState).makeMove(m);

            int value;
            if (nextTurn == mySide && !isFirstMove) {
                value = minMax(nextBoardState, mySide, true, alpha, beta, false, 1);
            }else {
                value = minMax(nextBoardState, mySide, false, alpha, beta, isFirstMove, 1);
            }

            if (value > bestMoveValue) {
                bestMoveValue = value;
                bestMove = m;
            }

            if (bestMoveValue > alpha) {
                alpha = bestMoveValue;
            }

            if (beta <= alpha) {
                break;
            }
        }

        logger.info("Best move is: " + bestMove.toString());

        String moveMsg;
        if (bestMove instanceof Move.SwapMove) {
            moveMsg = Protocol.createSwapMsg();
            mySide = mySide.opposite();
        }
        else {
            moveMsg = Protocol.createMoveMsg(bestMove.getHole());
        }

        System.out.print(moveMsg);
        System.out.flush();
    }


}
