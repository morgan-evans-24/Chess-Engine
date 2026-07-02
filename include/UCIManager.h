//
// Created by Mercury on 18/02/2026.
//

#ifndef CHESS_ENGINE_UCIMANAGER_H
#define CHESS_ENGINE_UCIMANAGER_H
#include "BoardManager.h"
#include "UCILogger.h"


class UCIManager {

    public:
        explicit UCIManager(BoardManager& boardManager);
        void prelogue();
        void readyUp();
        void readLine(std::string& line);
        void writeLine(std::string line);
        void mainLoop();
    private:
        UCILogger logger;
        BoardManager& boardManager;
        std::string startposAsFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
};


#endif //CHESS_ENGINE_UCIMANAGER_H