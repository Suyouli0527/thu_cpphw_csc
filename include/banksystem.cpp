#include "banksystem.h"
#include "command.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

BankSystem::BankSystem()
    : accountMgr(), userMgr(&accountMgr), logMgr() {}

BankSystem::~BankSystem() {
}

std::string BankSystem::formatAmount(double amount) {
    return AccountManager::formatAmount(amount);
}

bool BankSystem::requireAccount(int id) const {
    if (!accountMgr.findAccount(id)) {
        logMgr.printFailure();
        return false;
    }
    if (!userMgr.ownsAccount(id)) {
        logMgr.printFailure();
        return false;
    }
    return true;
}

bool BankSystem::requireAccount(int id, int accountPassword) const {
    Account* acc = accountMgr.findAccount(id);
    if (!acc || !userMgr.ownsAccount(id)) {
        logMgr.printFailure();
        return false;
    }
    if (!acc->verifyAccountPassword(accountPassword)) {
        logMgr.printFailure();
        return false;
    }
    return true;
}

bool BankSystem::requireAdmin() const {
    if (!userMgr.isAdmin()) {
        logMgr.printFailure();
        return false;
    }
    return true;
}

void BankSystem::logResult(bool ok) {
    if (ok) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::openAccount(int id, char type, const std::string &accountName, double balance, int repaymentDay, int accountPassword, bool shared) {
    if (id <= 0) { logMgr.printFailure(); return; }
    if (accountMgr.findAccount(id) != nullptr) { logMgr.printFailure(); return; }
    if (type != 'S' && type != 'C') { logMgr.printFailure(); return; }
    if (accountName.empty()) { logMgr.printFailure(); return; }
    if (balance < 0) { logMgr.printFailure(); return; }
    if (type == 'C' && (repaymentDay < 1 || repaymentDay > 28)) { logMgr.printFailure(); return; }
    if (accountPassword < 100000 || accountPassword > 999999) { logMgr.printFailure(); return; }

    Account* newAccount = nullptr;
    if (type == 'S') {
        newAccount = new SavingAccount(id, type, accountName, balance, accountMgr.getCurrentDate(), accountPassword, shared);
    } else {
        newAccount = new CreditAccount(id, type, accountName, balance, repaymentDay, accountMgr.getCurrentDate(), accountPassword, shared);
    }
    newAccount->addOwner(userMgr.getCurrentUserName());
    if (!accountMgr.addAccount(newAccount)) {
        delete newAccount;
        logMgr.printFailure();
        return;
    }
    userMgr.addAccountToUser(userMgr.getCurrentUserName(), id);
    logResult(true);
}

void BankSystem::closeAccount(int id, int accountPassword) {
    if (!requireAccount(id, accountPassword)) return;
    Account* acc = accountMgr.findAccount(id);
    if (acc->getBalance() != 0) { logMgr.printFailure(); return; }
    if (acc->getType() == 'S') {
        SavingAccount* sa = static_cast<SavingAccount*>(acc);
        if (sa->getFixedDepositCount() > 0) { logMgr.printFailure(); return; }
    }
    for (const auto &ownerName : acc->getOwners()) {
        userMgr.removeAccountFromUser(ownerName, id);
    }
    accountMgr.removeAccount(id);
    logResult(true);
}

void BankSystem::addOwner(int id, const std::string &userName) {
    if (!requireAdmin()) return;
    Account* acc = accountMgr.findAccount(id);
    if (!acc) { logMgr.printFailure(); return; }
    if (!acc->isShared()) { logMgr.printFailure(); return; }
    User* targetUser = userMgr.findUser(userName);
    if (!targetUser) { logMgr.printFailure(); return; }
    auto before = acc->getOwners().size();
    acc->addOwner(userName);
    if (acc->getOwners().size() == before) { logMgr.printFailure(); return; }
    targetUser->addAccountID(id);
    logResult(true);
}

void BankSystem::removeOwner(int id, const std::string &userName) {
    if (!requireAdmin()) return;
    Account* acc = accountMgr.findAccount(id);
    if (!acc) { logMgr.printFailure(); return; }
    if (!acc->isShared()) { logMgr.printFailure(); return; }
    auto before = acc->getOwners().size();
    acc->removeOwner(userName);
    if (acc->getOwners().size() == before) { logMgr.printFailure(); return; }
    User* targetUser = userMgr.findUser(userName);
    if (targetUser) targetUser->removeAccountID(id);
    logResult(true);
}

void BankSystem::modifyName(int id, const std::string &username, int accountPassword) {
    if (!requireAccount(id, accountPassword)) return;
    if (username.empty()) { logMgr.printFailure(); return; }
    accountMgr.findAccount(id)->modifyName(username);
    logResult(true);
}

void BankSystem::modifyCredit(int id, double newCredit, int accountPassword) {
    if (!requireAdmin()) return;
    if (!requireAccount(id, accountPassword)) return;
    Account* acc = accountMgr.findAccount(id);
    if (acc->getType() != 'C') { logMgr.printFailure(); return; }
    if (newCredit < 0) { logMgr.printFailure(); return; }
    static_cast<CreditAccount*>(acc)->modifyCredit(newCredit);
    logResult(true);
}

void BankSystem::modifyShared(int id, bool shared) {
    if (!requireAdmin()) return;
    Account* acc = accountMgr.findAccount(id);
    if (!acc) { logMgr.printFailure(); return; }
    acc->setShared(shared);
    logResult(true);
}

void BankSystem::changeUserPassword(const std::string &oldPassword, const std::string &newPassword) {
    User* user = userMgr.findUser(userMgr.getCurrentUserName());
    if (!user) { logMgr.printFailure(); return; }
    if (!user->changePassword(oldPassword, newPassword)) { logMgr.printFailure(); return; }
    logResult(true);
}

void BankSystem::changeAccountPassword(int id, int oldPassword, int newPassword) {
    if (!requireAccount(id, oldPassword)) return;
    accountMgr.findAccount(id)->changeAccountPassword(oldPassword, newPassword);
    logResult(true);
}

void BankSystem::query(int id, int accountPassword) const {
    Account* acc = accountMgr.findAccount(id);
    if (!acc || !userMgr.ownsAccount(id)) { logMgr.printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { logMgr.printFailure(); return; }
    accountMgr.printAccountDetail(id);
}

void BankSystem::queryAllAccounts() const {
    User* user = userMgr.findUser(userMgr.getCurrentUserName());
    if (!user) { logMgr.printFailure(); return; }
    std::vector<int> ids = user->getAccountIDs();
    if (ids.empty()) { logMgr.printFailure(); return; }
    std::sort(ids.begin(), ids.end());
    for (int id : ids) {
        accountMgr.printAccountInfo(id);
    }
}

void BankSystem::deposit(int id, double amount, int accountPassword) {
    if (!requireAccount(id)) return;
    logResult(accountMgr.deposit(id, amount, accountPassword));
}

void BankSystem::withdraw(int id, double amount, int accountPassword) {
    if (!requireAccount(id)) return;
    logResult(accountMgr.withdraw(id, amount, accountPassword));
}

void BankSystem::transfer(int srcId, int dstId, double amount, int srcAccountPassword) {
    if (!userMgr.ownsAccount(srcId)) { logMgr.printFailure(); return; }
    logResult(accountMgr.transfer(srcId, dstId, amount, srcAccountPassword));
}

void BankSystem::fixedDeposit(int id, double amount, int months, int accountPassword) {
    if (!requireAccount(id)) return;
    logResult(accountMgr.fixedDeposit(id, amount, months, accountPassword));
}

void BankSystem::fixedWithdraw(int id, double amount, int accountPassword) {
    if (!requireAccount(id)) return;
    logResult(accountMgr.fixedWithdraw(id, amount, accountPassword));
}

void BankSystem::consume(int id, double amount, int accountPassword) {
    if (!requireAccount(id)) return;
    logResult(accountMgr.consume(id, amount, accountPassword));
}

void BankSystem::showDate() const {
    if (!requireAdmin()) return;
    accountMgr.showDate();
}

void BankSystem::addDays(int days) {
    if (!requireAdmin()) return;
    logResult(accountMgr.addDays(days));
}

void BankSystem::setDate(int year, int month, int day) {
    if (!requireAdmin()) return;
    logResult(accountMgr.setDate(year, month, day));
}

void BankSystem::createUser(const std::string &username, const std::string &password) {
    logResult(userMgr.createUser(username, password));
}

void BankSystem::deleteUser(const std::string &username) {
    logResult(userMgr.deleteUser(username));
}

void BankSystem::queryUser(const std::string &username) const {
    if (!requireAdmin()) return;
    User* user = userMgr.findUser(username);
    if (!user) { logMgr.printFailure(); return; }
    std::vector<int> ids = user->getAccountIDs();
    if (ids.empty()) { logMgr.printFailure(); return; }
    std::sort(ids.begin(), ids.end());
    for (int id : ids) {
        accountMgr.printAccountInfo(id);
    }
}

void BankSystem::queryAllUser() const {
    if (!userMgr.queryAllUser()) {
        logMgr.printFailure();
    }
}

void BankSystem::switchUser(const std::string &username, const std::string &password) {
    logResult(userMgr.switchUser(username, password));
}

void BankSystem::whoami() const {
    userMgr.whoami();
}

void BankSystem::showLog() const {
    if (!requireAdmin()) return;
    logMgr.showLog();
}

void BankSystem::rollback(int n) {
    if (!requireAdmin()) return;
    if (n < 0 || n > (int)logMgr.getLogRecords().size()) {
        logMgr.printFailure();
        return;
    }

    std::vector<std::string> cmdsToReplay(logMgr.getLogRecords().begin(),
                                           logMgr.getLogRecords().begin() + n);

    accountMgr.clearAccounts();
    userMgr.reset();
    accountMgr.reset();
    logMgr.clear();

    replayCommands(cmdsToReplay);

    logMgr.printSuccess();
}

void BankSystem::saveLog(const std::string &filename) const {
    if (!requireAdmin()) return;
    logMgr.saveLog(filename);
}

void BankSystem::resume(const std::string &filename) {
    if (!requireAdmin()) return;
    if (!logMgr.isInitialState()) {
        logMgr.printFailure();
        return;
    }
    std::ifstream file(filename);
    if (!file.is_open()) {
        logMgr.printFailure();
        return;
    }
    std::vector<std::string> subCommands;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto pos = line.find(' ');
        if (pos == std::string::npos) {
            logMgr.printFailure();
            return;
        }
        subCommands.push_back(line.substr(pos + 1));
    }
    file.close();

    replayCommands(subCommands);

    logMgr.printSuccess();
}

void BankSystem::replayCommands(const std::vector<std::string>& commands) {
    logMgr.setSilent(true);
    std::string ignored;
    for (const auto &cmd : commands) {
        std::istringstream iss(cmd);
        std::string action;
        iss >> action;
        if (action == "QUERY" || action == "QUERYALL" || action == "QUERY_USER"
            || action == "QUERY_USERLIST" || action == "SHOW_DATE" || action == "WHOAMI"
            || action == "LOG" || action == "ROLLBACK" || action == "SAVE" || action == "RESUME") {
            continue;
        }
        Command::execute(*this, cmd, ignored);
    }
    logMgr.setSilent(false);
}
