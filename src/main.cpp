#include <fstream>
#include <iostream>

#include "../include/BoardManager.h"
#include "../include/DebugUtils.h"
#include "../include/UCIManager.h"

int main() {
    Zobrist::initialise();
    auto boardManager = BoardManager();
    auto uciManager = UCIManager(boardManager);
    uciManager.prelogue();
    uciManager.readyUp();
    uciManager.mainLoop();
}
