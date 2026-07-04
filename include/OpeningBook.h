//
// Created by Mercury on 04/07/2026.
//

#ifndef CHESS_ENGINE_OPENINGBOOK_H
#define CHESS_ENGINE_OPENINGBOOK_H
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>

#include "Move.h"
#include "Board.h"

namespace fs = std::filesystem;

class OpeningBook {
    public:
        static void initialize();
        static std::string lookup(uint64_t key);
        static int numOpeningBookMoves;
    private:
        static std::unordered_map<uint64_t, std::vector<std::string>> openingBook;
        static std::string startPosAsFen;
        static fs::path gamesPath;
        static std::ifstream gamesFile;
};


#endif //CHESS_ENGINE_OPENINGBOOK_H