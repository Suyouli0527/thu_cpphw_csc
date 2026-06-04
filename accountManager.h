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
    const std::vector<Account*>& getAccounts() const { return accounts; }

    void showDate() const;
    bool addDays(int days);
    bool setDate(int year, int month, int day);
    const Date& getCurrentDate() const { return currentDate; }
    void reset();

    bool deposit(int id, double amount);
    bool withdraw(int id, double amount);
    bool transfer(int srcId, int dstId, double amount);

    static const double savingRate;
    static const double creditRate;
    static const double debtRate;
    static double calcDailyInterest(char type, double balance, const Date &date);
};
