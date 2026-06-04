#pragma once
#include "date.h"
#include "user.h"
#include "account.h"
#include "accountManager.h"
#include "userManager.h"
#include "logManager.h"
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

class BankSystem {
private:
    AccountManager accountMgr;
    UserManager userMgr;
    LogManager logMgr;

    void replayCommands(const std::vector<std::string>& commands);
    bool requireAdmin() const;
    bool requireAccount(int id) const;
    bool requireAccount(int id, int accountPassword) const;
    void logResult(bool ok);

public:
    BankSystem();
    ~BankSystem();

    BankSystem(const BankSystem&) = delete;
    BankSystem& operator=(const BankSystem&) = delete;

    void openAccount(int id, char type, const std::string &accountName, double balance, int repaymentDay, int accountPassword, bool shared);
    void closeAccount(int id, int accountPassword);
    void addOwner(int id, const std::string &userName);
    void removeOwner(int id, const std::string &userName);
    void modifyName(int id, const std::string &username, int accountPassword);
    void modifyCredit(int id, double newCredit, int accountPassword);
    void modifyShared(int id, bool shared);
    void changeUserPassword(const std::string &oldPassword, const std::string &newPassword);
    void changeAccountPassword(int id, int oldPassword, int newPassword);
    void query(int id, int accountPassword) const;
    void queryAllAccounts() const;

    void deposit(int id, double amount, int accountPassword);
    void withdraw(int id, double amount, int accountPassword);
    void transfer(int srcId, int dstId, double amount, int srcAccountPassword);
    void fixedDeposit(int id, double amount, int months, int accountPassword);
    void fixedWithdraw(int id, double amount, int accountPassword);
    void consume(int id, double amount, int accountPassword);
    void cashAdvance(int id, double amount, int accountPassword);

    void showDate() const;
    void addDays(int days);
    void setDate(int year, int month, int day);

    void createUser(const std::string &username, const std::string &password);
    void deleteUser(const std::string &username);
    void queryUser(const std::string &username) const;
    void queryAllUser() const;
    void switchUser(const std::string &username, const std::string &password);
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
    std::string getCurrentUserName() const { return userMgr.getCurrentUserName(); }
    bool lastResult() const { return logMgr.lastResult(); }
    void resetResult() { logMgr.resetResult(); }
    static std::string formatAmount(double amount);
};
