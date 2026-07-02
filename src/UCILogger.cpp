//
// Created by Mercury on 01/07/2026.
//

#include "../include/UCILogger.h"

UCILogger::UCILogger() {
    fs::create_directory("logs");
    logFile.open(logPath, std::ios::app);
    auto timestamp = std::chrono::system_clock::now();
    std::time_t now_tt = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm = *std::localtime(&now_tt);
    logFile << "=========================================" << std::endl;
    logFile << " Log starts at time: " << std::put_time(&tm, "%Y-%m-%d_%H:%M:%S") << std::endl;
    logFile << "=========================================" << std::endl;
}

void UCILogger::logInput(const std::string& line) {
    if (!line.empty()) {
        logFile << "<< " << line << std::endl;
    }
}

void UCILogger::logOutput(const std::string& line) {
    if (!line.empty()) {
        logFile << ">> " << line << std::flush;
    }
}

void UCILogger::close() {
    auto timestamp = std::chrono::system_clock::now();
    std::time_t now_tt = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm = *std::localtime(&now_tt);
    logFile << "=========================================" << std::endl;
    logFile << "  Log ends at time: " << std::put_time(&tm, "%Y-%m-%d_%H:%M:%S") << std::endl;
    logFile << "=========================================" << std::endl;
    logFile.close();
}