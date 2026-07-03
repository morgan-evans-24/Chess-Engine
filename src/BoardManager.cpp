//
// Created by Mercury on 19/02/2026.
//

#include "../include/BoardManager.h"

#include <chrono>

#include "../include/DebugUtils.h"
#include "../include/MoveParser.h"

using std::string;

#include <array>

int BoardManager::depth = 6;

BoardManager::BoardManager() {
    board = Board();
    moveGenerator = MoveGenerator();
};

void BoardManager::newGame() {
    board.zobristHash = 0;
}
void BoardManager::setPosition(const string& FENString) {
    board.zobristHistory.clear();
    MoveParser::parseFenString(board, FENString);
}

void BoardManager::makeMove(const string& moveString) {
    MoveParser::parseMove(board, moveString);
}

void BoardManager::setDepth(const int setDepth) {
    depth = setDepth;
}

void BoardManager::setActiveColour(const Color colour) {
    board.whoseTurn = colour;
}

string BoardManager::search() {

    DebugUtils::startMoveTimer();

    Move bestMove = moveGenerator.getBestMove(board, depth);

    DebugUtils::printMoveTime();
    DebugUtils::printNumExploredPositions();


    // We can count the trailing zeros of a position to get what power of 2 it is
    // Much faster than using log2
    int startSquareTrailingZeros = __builtin_ctzll(bestMove.getStartSquare());
    int endSquareTrailingZeros = __builtin_ctzll(bestMove.getEndSquare());
    string returnString = string(squareMap[startSquareTrailingZeros]) + string(squareMap[endSquareTrailingZeros]);
    if (bestMove.isPromotion) {
        returnString += pieceCharMap[static_cast<int>(bestMove.promotionType)];
    }
    return returnString;
}


