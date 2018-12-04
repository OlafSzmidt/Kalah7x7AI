package MKAgent;

import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import static MKAgent.BotFunctions.minMax;

public class Bot {
    private Side mySide;
    private Scanner scanner = new Scanner(System.in);
    private Board boardState = new Board(7, 7);
    private boolean madeMoveYet = false;


    public static void main(String[] args) throws Exception {
        Bot b = new Bot();
        for(;;) {
            b.runBot();
        }
    }

    public void runBot() throws Exception {
        String input = scanner.nextLine();
        input += "\n";

        MsgType msgType = Protocol.getMessageType(input);

        switch (msgType) {
            case START:
                boolean amISouth = Protocol.interpretStartMsg(input);

                if (amISouth) {
                    mySide = Side.SOUTH;
                    makeMove(true, false);
                }
                else {
                    mySide = Side.NORTH;
                }

                break;


            case STATE:
                Protocol.MoveTurn moveTurn = Protocol.interpretStateMsg(input, boardState);

                if (moveTurn.again) {
                    makeMove(false, !madeMoveYet);
                }

		if (moveTurn.move == -1)
		{
			mySide = mySide.opposite();
		}

                break;

            case END:
                System.exit(-1);
        }


    }

    static class PlayResult {
        Move startMove;
        Future<Integer> task;

        PlayResult(Move s, Future<Integer> r) {
            startMove = s;
            task = r;
        }
    }

    public void makeMove(final boolean isFirstMove, boolean isSecondMove) throws Exception{
        madeMoveYet = true;

        List<Move> legalMoves = BotFunctions.getLegalMoves(boardState, mySide);


        Move bestMove = legalMoves.get(0);
        int bestMoveValue = Integer.MIN_VALUE;
        int alpha = Integer.MIN_VALUE;
        int beta = Integer.MAX_VALUE;

        ExecutorService executorService = Executors.newFixedThreadPool(4);

        List<PlayResult> results = new ArrayList<>();
        final int alphCopy = alpha;
        final int betaCopy = beta;
        // Simulate swap move
        if (isSecondMove) {
            Callable callable = new Callable<Integer>() {
                @Override
                public Integer call() throws Exception {
                    return minMax(boardState, mySide.opposite(), false, alphCopy, betaCopy, false, 1);
                }
            };

            results.add(new PlayResult(new Move.SwapMove(), executorService.submit(callable)));
        }

        for (Move m : legalMoves) {
            final Board nextBoardState = new Board(boardState);
            Side nextTurn = new Kalah(nextBoardState).makeMove(m);

            if (nextTurn == mySide && !isFirstMove) {
                Callable callable = new Callable<Integer>() {
                    @Override
                    public Integer call() throws Exception {
                        return minMax(nextBoardState, mySide, true, alphCopy, betaCopy, false, 1);
                    }
                };

                results.add(new PlayResult(m, executorService.submit(callable)));
            }else {
                Callable callable = new Callable<Integer>() {
                    @Override
                    public Integer call() throws Exception {
                        return minMax(nextBoardState, mySide, false, alphCopy, betaCopy, isFirstMove, 1);
                    }
                };

                results.add(new PlayResult(m, executorService.submit(callable)));
            }


        }

        for (PlayResult result : results) {
            if (result.task.isDone()) {
                int value = result.task.get();

                if (value > bestMoveValue) {
                    bestMoveValue = value;
                    bestMove = result.startMove;
                }

                if (bestMoveValue > alpha) {
                    alpha = bestMoveValue;
                }

                if (beta <= alpha) {
                    executorService.shutdownNow();
                    break;
                }
            }
        }

        String moveMsg;
        if (bestMove instanceof Move.SwapMove) {
            moveMsg = Protocol.createSwapMsg();
            mySide = mySide.opposite();
        }
        else {
            moveMsg = Protocol.createMoveMsg(bestMove.getHole());
        }

        System.out.print(moveMsg);
    }


}
