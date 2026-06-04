#pragma once
#include <vector>
#include <string>

class LogManager {
public:
    LogManager() = delete;

    static void recordLog(std::vector<std::string>& logRecords, const std::string& cmd);
    static void showLog(const std::vector<std::string>& logRecords, bool silent);
    static void printSuccess(bool silent);
    static void printFailure(bool silent);
    static bool isInitialState(const std::vector<std::string>& logRecords);
    static void saveLog(const std::vector<std::string>& logRecords, const std::string& filename, bool silent);
    static void clear(std::vector<std::string>& logRecords);
};
