#pragma once
#include "account.h"
#include "date.h"
#include <vector>
#include <string>

class AccountManager {
public:
    AccountManager() = delete;

    static Account* findAccount(const std::vector<Account*>& accounts, int id);
    static bool addAccount(std::vector<Account*>& accounts, Account* acc);
    static bool removeAccount(std::vector<Account*>& accounts, int id);
    static void clearAccounts(std::vector<Account*>& accounts);
    static void printAccountInfo(const std::vector<Account*>& accounts, int id);

    static const double savingRate;
    static const double creditRate;
    static const double debtRate;
    static double calcDailyInterest(char type, double balance, const Date &date);
    static void updateAllAccountsInterest(const std::vector<Account*>& accounts, const Date &newDate);

    static bool deposit(std::vector<Account*>& accounts, const Date &currentDate, int id, double amount);
    static bool withdraw(std::vector<Account*>& accounts, const Date &currentDate, int id, double amount);
    static bool transfer(std::vector<Account*>& accounts, const Date &currentDate, int srcId, int dstId, double amount);
};
