#pragma once
#include "date.h"
#include "user.h"
#include "account.h"
#include "userManager.h"
#include "logManager.h"
#include <vector>
#include <string>

class BankSystem {
private:
    std::vector<Account*> accounts;
    std::vector<User> users;
    std::string currentUserName;
    Date currentDate;
    std::vector<std::string> logRecords;
    bool m_silent;
    std::string m_currentCommand;

    void replayCommands(const std::vector<std::string>& commands);
    bool requireAdmin() const;
    bool requireAccount(int id) const;
    void logResult(bool ok);

public:
    BankSystem();
    ~BankSystem();

    BankSystem(const BankSystem&) = delete;
    BankSystem& operator=(const BankSystem&) = delete;

    void openAccount(int id, char type, const std::string &accountName, double balance);
    void closeAccount(int id);
    void modifyName(int id, const std::string &username);
    void modifyCredit(int id, double newCredit);
    void query(int id) const;
    void queryAllAccounts() const;

    void deposit(int id, double amount);
    void withdraw(int id, double amount);
    void transfer(int srcId, int dstId, double amount);

    void showDate() const;
    void addDays(int days);
    void setDate(int year, int month, int day);

    void createUser(const std::string &username);
    void deleteUser(const std::string &username);
    void queryUser(const std::string &username) const;
    void queryAllUser() const;
    void switchUser(const std::string &username);
    void whoami() const;

    void showLog() const;
    void rollback(int n);
    void saveLog(const std::string &filename) const;
    void resume(const std::string &filename);

    bool isLoggedIn() const { return UserManager::isLoggedIn(currentUserName); }
    bool isAdmin() const { return UserManager::isAdmin(users, currentUserName); }
    bool isLegalName(const std::string &name) const { return UserManager::isLegalName(name); }
    bool isInitialState() const { return LogManager::isInitialState(logRecords); }
    void setSilent(bool s) { m_silent = s; }
    void setRawCommand(const std::string &cmd) { m_currentCommand = cmd; }
    const std::string& getCurrentCommand() const { return m_currentCommand; }
};
