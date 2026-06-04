#pragma once
#include "tools.h"
#include <vector>
#include <string>

class LogManager {
private:
    std::vector<std::string> logRecords;
    bool m_silent = false;
    std::string m_currentCommand;

public:
    void recordLog(const std::string &cmd);
    void showLog() const;
    void printSuccess() const;
    void printFailure() const;

    void setSilent(bool s) { m_silent = s; }
    void setRawCommand(const std::string &cmd) { m_currentCommand = cmd; }
    bool isInitialState() const { return logRecords.empty(); }
    const std::string& getCurrentCommand() const { return m_currentCommand; }

    void saveLog(const std::string &filename) const;
    const std::vector<std::string>& getLogRecords() const { return logRecords; }
    void clear();
};
