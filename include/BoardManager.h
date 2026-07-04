//
// Created by Mercury on 19/02/2026.
//

#ifndef CHESS_ENGINE_BOARDMANAGER_H
#define CHESS_ENGINE_BOARDMANAGER_H
#include <string>

#include "Board.h"
#include "MoveParser.h"
#include "MoveGenerator.h"
#include "OpeningBook.h"


class BoardManager {
    public:
        BoardManager();
        void newGame();
        void setPosition(const std::string& FENString);
        void makeMove(const std::string& moveString);
        static void setDepth(int setDepth);
        void setActiveColour(Color colour);
        std::string search();
        std::string pollOpenBook() const;
        static inline int fullMoveClock;
        static inline int halfMoveClock;
        static int depth;
    private:
        Board board;
        MoveGenerator moveGenerator;
        OpeningBook openingBook;
        static constexpr std::array<const char*, 64> squareMap = {
            "a1","b1","c1","d1","e1","f1","g1","h1",
            "a2","b2","c2","d2","e2","f2","g2","h2",
            "a3","b3","c3","d3","e3","f3","g3","h3",
            "a4","b4","c4","d4","e4","f4","g4","h4",
            "a5","b5","c5","d5","e5","f5","g5","h5",
            "a6","b6","c6","d6","e6","f6","g6","h6",
            "a7","b7","c7","d7","e7","f7","g7","h7",
            "a8","b8","c8","d8","e8","f8","g8","h8"
        };
        static constexpr std::array<const char, 12> pieceCharMap = {
            '-', 'n', 'b', 'r', 'q', '-', '-', 'n', 'b', 'r', 'q', '-'
        };
};


#endif //CHESS_ENGINE_BOARDMANAGER_H