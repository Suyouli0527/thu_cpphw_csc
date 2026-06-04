#pragma once
#include "date.h"
#include "user.h"
#include "account.h"
#include "tools.h"
#include "accountManager.h"
#include "userManager.h"
#include "dateManager.h"
#include "transactionManager.h"
#include "logManager.h"
#include <vector>
#include <string>

class BankSystem {
private:
    AccountManager accountMgr;
    UserManager userMgr;
    DateManager dateMgr;
    LogManager logMgr;
    TransactionManager transactionMgr;

    void replayCommands(const std::vector<std::string>& commands);

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

    bool isLoggedIn() const { return userMgr.isLoggedIn(); }
    bool isAdmin() const { return userMgr.isAdmin(); }
    bool isLegalName(const std::string &name) const { return UserManager::isLegalName(name); }
    bool isInitialState() const { return logMgr.isInitialState(); }
    void setSilent(bool s) { logMgr.setSilent(s); }
    void setRawCommand(const std::string &cmd) { logMgr.setRawCommand(cmd); }
};
