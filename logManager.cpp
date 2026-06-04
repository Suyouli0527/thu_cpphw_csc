#include "logManager.h"
#include "tools.h"
#include <iostream>
#include <fstream>

void LogManager::recordLog(std::vector<std::string>& logRecords, const std::string& cmd) {
    logRecords.push_back(cmd);
}

void LogManager::showLog(const std::vector<std::string>& logRecords, bool silent) {
    if (logRecords.empty()) {
        printFailure(silent);
        return;
    }
    for (size_t i = 0; i < logRecords.size(); i++) {
        std::cout << (i + 1) << " " << logRecords[i] << std::endl;
    }
}

void LogManager::printSuccess(bool silent) {
    if (!silent) Tools::printSuccess();
}

void LogManager::printFailure(bool silent) {
    if (!silent) Tools::printFailure();
}

bool LogManager::isInitialState(const std::vector<std::string>& logRecords) {
    return logRecords.empty();
}

void LogManager::saveLog(const std::vector<std::string>& logRecords, const std::string& filename, bool silent) {
    std::ofstream file(filename);
    if (!file) {
        printFailure(silent);
        return;
    }
    for (size_t i = 0; i < logRecords.size(); i++) {
        file << (i + 1) << " " << logRecords[i] << std::endl;
    }
    printSuccess(silent);
}

void LogManager::clear(std::vector<std::string>& logRecords) {
    logRecords.clear();
}
