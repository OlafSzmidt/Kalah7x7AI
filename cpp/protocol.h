//
// Created by laptop on 08/12/2018.
//

#ifndef UNTITLED1_PROTOCOL_H
#define UNTITLED1_PROTOCOL_H

#include <string>
#include <sstream>
#include <vector>

enum MsgType {START, STATE, END};

struct MoveTurn {
    bool end;
    bool again;
    int move;
};

std::string createMoveMsg(int hole) {
    std::stringstream in;

    in << "MOVE;" << hole << "\n";

    return in.str();
}

std::string createSwapMsg() {
    return std::string("SWAP\n");
}

bool stringStartsWith(const std::string& bigString, const std::string& smallString) {
    return bigString.compare(0, smallString.length(), smallString) == 0;
}

MsgType getMessageType(const std::string& msg) {
    std::string startString("START");
    std::string changeString("CHANGE;");
    std::string endString("END\n");

    if (stringStartsWith(msg, startString)) {
        return MsgType (START);
    }
    else if (stringStartsWith(msg, changeString)) {
        return MsgType (STATE);
    }
    else if (stringStartsWith(msg, endString)) {
        return MsgType (END);
    }
    else {
        throw "Could not determine msg type\n";
    }
}

std::vector<std::string> split(const std::string& input, char delim, int limit) {
    std::string part;
    std::istringstream in(input);
    std::vector<std::string> msgParts;

    int i = 0;
    while ((i < limit || limit < 0) && std::getline(in, part, delim)) {
        msgParts.push_back(part);
        ++i;
    }

    return msgParts;
}

bool interpretStartMsg (const std::string& msg)  {
    uint32_t msgLength = msg.size();

    if (msg.at(msgLength - 1) != '\n') {
        std::cerr << "Message not terminated with 0x0A character.";
        throw "Message not terminated with 0x0A character.";
    }

    std::string pos = msg.substr(6, 5);

    std::string southString("South");

    if (pos.compare(southString) == 0) {
        return true;
    }
    else if (pos.compare("North") == 0) {
        return false;
    }
    else {
        std::cerr << "Illegal position parameter\n" << pos << "\n";
        std::cerr << "Length: " << pos.size() << "\n";
        throw "Illegal position parameter";
    }

}

MoveTurn interpretStateMsg (const std::string& msg, Board& board)
{
        MoveTurn moveTurn;

        uint32_t msgLength = msg.size();

        if (msg.at(msgLength - 1) != '\n') {
            throw "Message not terminated with 0x0A character.";
        }

    std::vector<std::string> msgParts(split(msg, ';', 4));


        if (msgParts.size() != 4)
            throw "Missing arguments.";

    // msgParts[0] is "CHANGE"

        // 1st argument: the move (or swap)

        if (msgParts.at(1).compare("SWAP") == 0)
        moveTurn.move = -1;
        else
        {
            moveTurn.move = std::stoi(msgParts[1]);
        }


    std::vector<std::string> boardParts(split(msgParts.at(2), ',', -1));


        /*if (boardParts.length % 2 != 0)
            throw new InvalidMessageException("Malformed board: odd number of entries.");*/
        if (2*(board.getNoOfHoles()+1) != boardParts.size())
        throw "Board dimensions in message are not as expected";


        // holes on the north side:
        for (int i = 0; i < board.getNoOfHoles(); i++)
            board.setSeeds(Side(NORTH), i+1, std::stoi(boardParts[i]));
        // northern store:
        board.setSeedsInStore(Side(NORTH), std::stoi(boardParts[board.getNoOfHoles()]));
        // holes on the south side:
        for (int i = 0; i < board.getNoOfHoles(); i++)
            board.setSeeds(Side(SOUTH), i+1, std::stoi(boardParts[i+board.getNoOfHoles()+1]));
        // southern store:
        board.setSeedsInStore(Side(SOUTH), std::stoi(boardParts[2*board.getNoOfHoles()+1]));

        // 3rd argument: who's turn?
        moveTurn.end = false;
        if (msgParts[3].compare("YOU\n") == 0)
        moveTurn.again = true;
        else if (msgParts[3].compare("OPP\n") == 0)
        moveTurn.again = false;
        else if (msgParts[3].compare("END\n") == 0)
        {
            moveTurn.end = true;
            moveTurn.again = false;
        }
        else
        throw "Illegal value for turn parameter: " + msgParts[3];

    return moveTurn;
}

#endif //UNTITLED1_PROTOCOL_H
