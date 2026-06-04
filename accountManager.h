#pragma once
#include "account.h"
#include <vector>
#include <string>

class AccountManager {
private:
    std::vector<Account*> accounts;

public:
    AccountManager() = default;
    ~AccountManager();

    AccountManager(const AccountManager&) = delete;
    AccountManager& operator=(const AccountManager&) = delete;

    Account* findAccount(int id) const;
    bool addAccount(Account* acc);
    bool removeAccount(int id);
    void clearAccounts();

    void printAccountInfo(int id) const;
    const std::vector<Account*>& getAccounts() const { return accounts; }
};
