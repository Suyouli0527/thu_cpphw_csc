#pragma once
#include "date.h"
#include "user.h"
#include "account.h"
#include "tools.h"
#include <vector>
#include <string>

class BankSystem {
private:
    Date currentDate;
    std::string currentUserName;
    std::vector<User> users;
    std::vector<Account*> accounts;
    std::vector<std::string> logRecords;
    bool m_silent = false;
    std::string m_currentCommand;

    Account* findAccount(int id) const;
    User* findUser(const std::string &name) const;
    void updateAllAccountsInterest(const Date &newDate);
    void removeAccount(int id);
    void printSuccess() const;
    void printFailure() const;
    bool ownsAccount(int id) const;
    void printAccountInfo(int id) const;

public:
    BankSystem();
    ~BankSystem();
    void clearAccounts();

    BankSystem(const BankSystem&) = delete;
    BankSystem& operator=(const BankSystem&) = delete;

    void openAccount(int id, char type, const std::string &accountName, double balance, int repaymentDay, int accountPassword);
    void closeAccount(int id, int accountPassword);
    void modifyName(int id, const std::string &username, int accountPassword);
    void modifyCredit(int id, double newCredit, int accountPassword);
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

    bool isLoggedIn() const { return !currentUserName.empty(); }
    bool isAdmin() const;
    bool isLegalName(const std::string &name) const;
    bool isInitialState() const { return logRecords.empty(); }
    void setSilent(bool s) { m_silent = s; }
    void setRawCommand(const std::string &cmd) { m_currentCommand = cmd; }
};
