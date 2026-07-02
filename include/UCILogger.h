//
// Created by Mercury on 01/07/2026.
//

#ifndef CHESS_ENGINE_UCILOGGER_H
#define CHESS_ENGINE_UCILOGGER_H
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

class UCILogger {
    public:
        UCILogger();
        void close();
        void logInput(const std::string&);
        void logOutput(const std::string&);
    private:
        fs::path logPath = fs::current_path() / "logs" / "log.txt";
        std::ofstream logFile;
};


#endif //CHESS_ENGINE_UCILOGGER_H