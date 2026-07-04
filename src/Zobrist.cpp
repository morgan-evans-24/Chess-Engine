//
// Created by Mercury on 01/07/2026.
//

#include "../include/Zobrist.h"

#include <random>

uint64_t Zobrist::pieceKeys[12][64];
uint64_t Zobrist::castleKeys[16];
uint64_t Zobrist::enPassantKeys[64];
uint64_t Zobrist::blackToMoveKey;

void Zobrist::initialise() {
    std::mt19937_64 rng(0x123456789ABCDEF);

    for (auto &piece : pieceKeys) {
        for (auto &square : piece) {
            square = rng();
        }
    }

    blackToMoveKey = rng();

    for (auto &castleRights : castleKeys) {
        castleRights = rng();
    }

    for (auto &enPassantSquare : enPassantKeys) {
        enPassantSquare = rng();
    }
}
