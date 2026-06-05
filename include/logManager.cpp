#include "logManager.h"
#include <iostream>
#include <fstream>

void LogManager::recordLog(const std::string &cmd) {
    logRecords.push_back(cmd);
}

void LogManager::showLog() const {
    if (logRecords.empty()) {
        printFailure();
        return;
    }
    for (auto i = 0; i < (int)logRecords.size(); i++) {
        std::cout << (i + 1) << " " << logRecords[i] << std::endl;
    }
}

void LogManager::printSuccess() const {
    m_lastResult = true;
    if (!m_silent) std::cout << '1' << std::endl;
}

void LogManager::printFailure() const {
    m_lastResult = false;
    if (!m_silent) std::cout << '0' << std::endl;
}

void LogManager::saveLog(const std::string &filename) const {
    std::ofstream file(filename);
    if (!file) {
        printFailure();
        return;
    }
    for (auto i = 0; i < (int)logRecords.size(); i++) {
        file << (i + 1) << " " << logRecords[i] << std::endl;
    }
    printSuccess();
}

void LogManager::clear() {
    logRecords.clear();
}
