#include <fstream>
#include <iostream>

#include "../include/BoardManager.h"
#include "../include/DebugUtils.h"
#include "../include/UCIManager.h"

int main() {
    Zobrist::initialise();
    OpeningBook::initialize();
    auto boardManager = BoardManager();
    auto uciManager = UCIManager(boardManager);
    uciManager.prelogue();
    uciManager.readyUp();
    uciManager.mainLoop();
}
