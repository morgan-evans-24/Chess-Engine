//
// Created by Mercury on 24/02/2026.
//

#ifndef CHESS_ENGINE_FENPARSER_H
#define CHESS_ENGINE_FENPARSER_H
#include <string>
#include <unordered_map>

#include "Board.h"


class MoveParser {
    public:
        static void parseFenString(Board& board, const std::string& FENString);
        static void parseMove(Board& board, const std::string& moveString);
        static std::string PGNToUCI(Board& board, std::string pgn);
    private:
        static std::unordered_map<char, Piece> charToPieceMap;
};


#endif //CHESS_ENGINE_FENPARSER_H