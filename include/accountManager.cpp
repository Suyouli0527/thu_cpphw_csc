#include "accountManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>

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

std::string AccountManager::formatAmount(double amount) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << amount;
    return oss.str();
}

void AccountManager::printAccountInfo(int id) const {
    Account* acc = findAccount(id);
    if (!acc) return;

    std::cout << acc->getId() << " "
              << acc->getType() << " "
              << acc->getName() << " "
              << formatAmount(acc->getBalance());
    if (acc->isShared()) {
        std::cout << " SHARED";
        for (const auto &o : acc->getOwners()) {
            std::cout << " " << o;
        }
    } else {
        std::cout << " NOT_SHARED";
    }
    if (acc->getType() == 'C') {
        const CreditAccount* creditAcc = static_cast<const CreditAccount*>(acc);
        std::cout << " " << formatAmount(creditAcc->getCredit())
                  << " " << creditAcc->getRepaymentDay()
                  << " " << formatAmount(creditAcc->getCashAdvanceDebt())
                  << " " << formatAmount(creditAcc->getConsumeDebt());
    }
    if (acc->getType() == 'S') {
        const SavingAccount* savingAcc = static_cast<const SavingAccount*>(acc);
        std::cout << " " << savingAcc->getFixedDepositCount();
        for (const auto& fd : savingAcc->getFixedDeposits()) {
            std::cout << " " << fd.months << " "
                      << formatAmount(fd.principal) << " "
                      << fd.depositDate.getYear() << "-"
                      << fd.depositDate.getMonth() << "-"
                      << fd.depositDate.getDay() << " "
                      << fd.maturityDate.getYear() << "-"
                      << fd.maturityDate.getMonth() << "-"
                      << fd.maturityDate.getDay() << " "
                      << (fd.partiallyWithdrawn ? "1" : "0");
        }
    }
    std::cout << std::endl;
}

void AccountManager::printAccountDetail(int id) const {
    Account* acc = findAccount(id);
    if (!acc) return;

    std::cout << "  账户ID: " << acc->getId() << std::endl;
    std::cout << "  类型: " << (acc->getType() == 'S' ? "储蓄账户" : "信用账户") << std::endl;
    std::cout << "  名称: " << acc->getName() << std::endl;
    std::cout << "  余额: " << formatAmount(acc->getBalance()) << std::endl;
    std::cout << "  共享: " << (acc->isShared() ? "是" : "否");
    if (acc->isShared()) {
        std::cout << "  共有人:";
        for (const auto &o : acc->getOwners()) {
            std::cout << " " << o;
        }
    }
    std::cout << std::endl;

    if (acc->getType() == 'C') {
        const CreditAccount* creditAcc = static_cast<const CreditAccount*>(acc);
        std::cout << "  信用额度: " << formatAmount(creditAcc->getCredit()) << std::endl;
        std::cout << "  还款日: 每月" << creditAcc->getRepaymentDay() << "日" << std::endl;
        std::cout << "  取现债务: " << formatAmount(creditAcc->getCashAdvanceDebt()) << std::endl;
        std::cout << "  消费债务: " << formatAmount(creditAcc->getConsumeDebt()) << std::endl;
    }

    if (acc->getType() == 'S') {
        const SavingAccount* savingAcc = static_cast<const SavingAccount*>(acc);

        std::cout << "  定期存款: " << savingAcc->getFixedDepositCount() << "笔" << std::endl;
        for (auto i = 0; i < (int)savingAcc->getFixedDeposits().size(); i++) {
            const FixedDeposit& fd = savingAcc->getFixedDeposits()[i];
            std::cout << "    [" << i << "] 本金:" << formatAmount(fd.principal)
                      << " 期限:" << fd.months << "月"
                      << " 存入:" << fd.depositDate.getYear() << "-"
                      << fd.depositDate.getMonth() << "-" << fd.depositDate.getDay()
                      << " 到期:" << fd.maturityDate.getYear() << "-"
                      << fd.maturityDate.getMonth() << "-" << fd.maturityDate.getDay()
                      << (fd.partiallyWithdrawn ? " [已部分支取]" : "")
                      << std::endl;
        }
    }
}
