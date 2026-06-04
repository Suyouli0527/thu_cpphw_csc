#include "accountManager.h"
#include "tools.h"
#include <iostream>

AccountManager::~AccountManager() {
    clearAccounts();
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
