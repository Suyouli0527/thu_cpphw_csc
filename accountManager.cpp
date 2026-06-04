#include "accountManager.h"
#include "tools.h"
#include <iostream>

const double AccountManager::savingRate = 0.0115;
const double AccountManager::creditRate = 0.0225;
const double AccountManager::debtRate = 0.0005;

double AccountManager::calcDailyInterest(char type, double balance, const Date &date) {
    double dailyRate;
    switch (type) {
        case 'S':
            dailyRate = savingRate / date.daysInYear();
            break;
        case 'C':
            if (balance >= 0) {
                dailyRate = creditRate / date.daysInYear();
            } else {
                dailyRate = debtRate;
            }
            break;
        default:
            return 0;
    }
    return balance * dailyRate;
}

void AccountManager::updateAllAccountsInterest(const std::vector<Account*>& accounts, const Date &newDate) {
    for (auto acc : accounts) {
        acc->updateInterest(newDate);
    }
}

Account* AccountManager::findAccount(const std::vector<Account*>& accounts, int id) {
    for (auto acc : accounts) {
        if (acc->getId() == id) {
            return acc;
        }
    }
    return nullptr;
}

bool AccountManager::addAccount(std::vector<Account*>& accounts, Account* acc) {
    if (!acc) return false;
    if (findAccount(accounts, acc->getId()) != nullptr) return false;
    accounts.push_back(acc);
    return true;
}

bool AccountManager::removeAccount(std::vector<Account*>& accounts, int id) {
    for (auto it = accounts.begin(); it != accounts.end(); ++it) {
        if ((*it)->getId() == id) {
            delete *it;
            accounts.erase(it);
            return true;
        }
    }
    return false;
}

void AccountManager::clearAccounts(std::vector<Account*>& accounts) {
    for (auto acc : accounts) {
        delete acc;
    }
    accounts.clear();
}

void AccountManager::printAccountInfo(const std::vector<Account*>& accounts, int id) {
    Account* acc = findAccount(accounts, id);
    if (!acc) return;
    std::cout << acc->getId() << " "
              << acc->getType() << " "
              << acc->getName() << " "
              << Tools::formatAmount(acc->getBalance());
    if (acc->getType() == 'C') {
        const CreditAccount* creditAcc = static_cast<const CreditAccount*>(acc);
        std::cout << " " << Tools::formatAmount(creditAcc->getCredit());
    }
    std::cout << std::endl;
}

bool AccountManager::deposit(std::vector<Account*>& accounts, const Date &currentDate, int id, double amount) {
    Account* acc = findAccount(accounts, id);
    if (!acc) return false;
    return acc->deposit(currentDate, amount);
}

bool AccountManager::withdraw(std::vector<Account*>& accounts, const Date &currentDate, int id, double amount) {
    Account* acc = findAccount(accounts, id);
    if (!acc) return false;
    return acc->withdraw(currentDate, amount);
}

bool AccountManager::transfer(std::vector<Account*>& accounts, const Date &currentDate, int srcId, int dstId, double amount) {
    if (srcId == dstId) return false;
    Account* srcAcc = findAccount(accounts, srcId);
    Account* dstAcc = findAccount(accounts, dstId);
    if (!srcAcc || !dstAcc) return false;
    if (!srcAcc->withdraw(currentDate, amount)) return false;
    dstAcc->deposit(currentDate, amount);
    return true;
}
