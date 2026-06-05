#pragma once
#include "account.h"
#include "date.h"
#include <vector>
#include <string>

class AccountManager {
private:
    std::vector<Account*> accounts;
    Date currentDate;

    void updateAllAccountsInterest(const Date &newDate);

public:
    AccountManager();
    ~AccountManager();

    AccountManager(const AccountManager&) = delete;
    AccountManager& operator=(const AccountManager&) = delete;

    Account* findAccount(int id) const;
    bool addAccount(Account* acc);
    bool removeAccount(int id);
    void clearAccounts();
    void printAccountInfo(int id) const;
    void printAccountDetail(int id) const;
    const std::vector<Account*>& getAccounts() const { return accounts; }
    static std::string formatAmount(double amount);

    void showDate() const;
    bool addDays(int days);
    bool setDate(int year, int month, int day);
    const Date& getCurrentDate() const { return currentDate; }
    void reset();

    bool deposit(int id, double amount, const std::string &accountPassword);
    bool withdraw(int id, double amount, const std::string &accountPassword);
    bool transfer(int srcId, int dstId, double amount, const std::string &srcAccountPassword);
    bool fixedDeposit(int id, double amount, int months, const std::string &accountPassword, bool autoRenew);
    bool fixedWithdraw(int id, double amount, const std::string &accountPassword);
    bool consume(int id, double amount, const std::string &accountPassword);


};
