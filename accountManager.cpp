#include "accountManager.h"
#include "tools.h"
#include <iostream>

AccountManager::AccountManager() : currentDate(1970, 1, 1) {}

AccountManager::~AccountManager() {
    clearAccounts();
}

void AccountManager::updateAllAccountsInterest(const Date &newDate) {
    for (auto acc : accounts) {
        acc->updateInterest(newDate);
    }
}

Account* AccountManager::findAccount(int id) const {
    for (auto acc : accounts) {
        if (acc->getId() == id) {
            return acc;
        }
    }
    return nullptr;
}

bool AccountManager::addAccount(Account* acc) {
    if (!acc) return false;
    if (findAccount(acc->getId()) != nullptr) return false;
    accounts.push_back(acc);
    return true;
}

bool AccountManager::removeAccount(int id) {
    for (auto it = accounts.begin(); it != accounts.end(); ++it) {
        if ((*it)->getId() == id) {
            delete *it;
            accounts.erase(it);
            return true;
        }
    }
    return false;
}

void AccountManager::clearAccounts() {
    for (auto acc : accounts) {
        delete acc;
    }
    accounts.clear();
}

void AccountManager::printAccountInfo(int id) const {
    Account* acc = findAccount(id);
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

void AccountManager::showDate() const {
    currentDate.showDate();
}

bool AccountManager::addDays(int days) {
    if (!currentDate.addDays(days)) return false;
    updateAllAccountsInterest(currentDate);
    return true;
}

bool AccountManager::setDate(int year, int month, int day) {
    if (!currentDate.setDate(year, month, day)) return false;
    updateAllAccountsInterest(currentDate);
    return true;
}

void AccountManager::reset() {
    currentDate = Date(1970, 1, 1);
}

bool AccountManager::deposit(int id, double amount) {
    Account* acc = findAccount(id);
    if (!acc) return false;
    return acc->deposit(currentDate, amount);
}

bool AccountManager::withdraw(int id, double amount) {
    Account* acc = findAccount(id);
    if (!acc) return false;
    return acc->withdraw(currentDate, amount);
}

bool AccountManager::transfer(int srcId, int dstId, double amount) {
    if (srcId == dstId) return false;
    Account* srcAcc = findAccount(srcId);
    Account* dstAcc = findAccount(dstId);
    if (!srcAcc || !dstAcc) return false;
    if (!srcAcc->withdraw(currentDate, amount)) return false;
    dstAcc->deposit(currentDate, amount);
    return true;
}
