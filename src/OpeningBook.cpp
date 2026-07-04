//
// Created by Mercury on 04/07/2026.
//

#include "../include/OpeningBook.h"

#include <iostream>


#include "../include/MoveParser.h"

std::unordered_map<uint64_t, std::vector<std::string>> OpeningBook::openingBook;
std::string OpeningBook::startPosAsFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
int OpeningBook::numOpeningBookMoves = 12;
fs::path OpeningBook::gamesPath = fs::current_path() / "games" / "games_uci.txt";
std::ifstream OpeningBook::gamesFile;

void OpeningBook::initialize() {
    auto board = Board();

    // Open games file
    gamesFile.open(gamesPath);
    std::string line;
    int numGamesRead = 0;
    char delimiter = ' ';
    while (getline(gamesFile, line)) {

        board.zobristHash = 0;
        MoveParser::parseFenString(board, startPosAsFen);

        std::istringstream ss = std::istringstream(line);
        int numMovesRead = 0;
        std::string uciMove;

        while (getline(ss, uciMove, delimiter) && numMovesRead < numOpeningBookMoves) {
            uint64_t key = board.zobristHash;

            MoveParser::parseMove(board, uciMove);

            openingBook[key].push_back(uciMove);
            numMovesRead++;
        }
        numGamesRead++;
    }
    if (numGamesRead == 0) {
        std::cerr << "No games found for opening book!" << std::endl;
    }
}

std::string OpeningBook::lookup(uint64_t key) {
    if (openingBook.find(key) == openingBook.end()) {
        return "NOMOVE";
    }

    std::vector<std::string> moves = openingBook[key];
    int numElements = moves.size();
    int randomIndex = rand() % numElements;
    return moves[randomIndex];
}
