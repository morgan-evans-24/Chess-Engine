//
// Created by Mercury on 01/07/2026.
//

#ifndef CHESS_ENGINE_ZOBRIST_H
#define CHESS_ENGINE_ZOBRIST_H
#include <cstdint>


class Zobrist {
    public:
        static void initialise();
        static uint64_t pieceKeys[12][64];
        static uint64_t blackToMoveKey;
        static uint64_t castleKeys[16];
        static uint64_t enPassantKeys[64];
};


#endif //CHESS_ENGINE_ZOBRIST_H