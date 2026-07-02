//
// Created by Mercury on 18/02/2026.
//

#include "../include/UCIManager.h"

#include <iostream>
#include <sstream>
#include <string>

#include "../include/DebugUtils.h"

using std::string;

UCIManager::UCIManager(BoardManager& boardManager) : boardManager(boardManager) {
    this->boardManager = boardManager;
}


void UCIManager::prelogue() {
    string uci_input;
    while (uci_input != "uci") {
        readLine(uci_input);
        if (uci_input == "quit") {
            logger.close();
            exit(0);
        }
    }
    writeLine("id name Chess-Engine\n");
    writeLine("id author Morgan\n");
    writeLine("uciok\n");
}

void UCIManager::readyUp() {
    string uci_input;
    while (uci_input != "isready") {
        // setoption calls go here
        readLine(uci_input);
        if (uci_input == "quit") {
            logger.close();
            exit(0);
        }
    }
    writeLine("readyok\n");
}

void UCIManager::readLine(string& line) {
    getline(std::cin, line);
    logger.logInput(line);
}

void UCIManager::writeLine(string line) {
    std::cout << line << std::flush;
    logger.logOutput(line);
}

void UCIManager::mainLoop() {
    string uci_input;
    string temp;
    std::istringstream ss = std::istringstream(uci_input);
    char delimiter = ' ';
    while (true) {
        readLine(uci_input);
        ss = std::istringstream(uci_input);

        getline(ss, temp, delimiter);

        if (uci_input == "ucinewgame") {
            // Clear board using board manager
            boardManager.newGame();
            continue;
        }

        if (temp == "position") {
            getline(ss, temp, delimiter);
            if (temp == "startpos") {
                boardManager.setPosition(startposAsFen);
            } else if (temp == "fen") {
                getline(ss, temp);
                boardManager.setPosition(temp);
            }
            getline(ss, temp, delimiter);
            if (temp == "moves") {
                int movecount = 0;
                getline(ss, temp, delimiter);
                while (!temp.empty()) {
                    boardManager.makeMove(temp);
                    temp.clear();
                    getline(ss, temp, delimiter);
                }
            }

        }

        if (temp == "go") {
            while (getline(ss, temp, delimiter)) {
                if (temp == "movetime") {
                    getline(ss, temp, delimiter);
                    // Do something with movetime
                } else if (temp == "depth") {
                    getline(ss, temp, delimiter);
                    const int depth = std::stoi(temp);
                    boardManager.setDepth(depth);
                }
            }
            string bestMove = boardManager.search();
            std::string response =  "bestmove " + bestMove + '\n';
            writeLine(response);
        }

        if (uci_input == "quit") {
            logger.close();
            exit(0);
        }
    }
}
