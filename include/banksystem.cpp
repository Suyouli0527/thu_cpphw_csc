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
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
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
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
}

void BankSystem::addOwner(int id, const std::string &userName) {
    if (!userMgr.isAdmin()) { logMgr.printFailure(); return; }
    Account* acc = accountMgr.findAccount(id);
    if (!acc) { logMgr.printFailure(); return; }
    if (!acc->isShared()) { logMgr.printFailure(); return; }
    User* targetUser = userMgr.findUser(userName);
    if (!targetUser) { logMgr.printFailure(); return; }
    auto before = acc->getOwners().size();
    acc->addOwner(userName);
    if (acc->getOwners().size() == before) { logMgr.printFailure(); return; }
    targetUser->addAccountID(id);
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
}

void BankSystem::removeOwner(int id, const std::string &userName) {
    if (!userMgr.isAdmin()) { logMgr.printFailure(); return; }
    Account* acc = accountMgr.findAccount(id);
    if (!acc) { logMgr.printFailure(); return; }
    if (!acc->isShared()) { logMgr.printFailure(); return; }
    auto before = acc->getOwners().size();
    acc->removeOwner(userName);
    if (acc->getOwners().size() == before) { logMgr.printFailure(); return; }
    User* targetUser = userMgr.findUser(userName);
    if (targetUser) targetUser->removeAccountID(id);
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
}

void BankSystem::modifyName(int id, const std::string &username, int accountPassword) {
    if (!requireAccount(id, accountPassword)) return;
    if (username.empty()) { logMgr.printFailure(); return; }
    accountMgr.findAccount(id)->modifyName(username);
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
}

void BankSystem::modifyCredit(int id, double newCredit, int accountPassword) {
    if (!userMgr.isAdmin()) { logMgr.printFailure(); return; }
    if (!requireAccount(id, accountPassword)) return;
    Account* acc = accountMgr.findAccount(id);
    if (acc->getType() != 'C') { logMgr.printFailure(); return; }
    if (newCredit < 0) { logMgr.printFailure(); return; }
    static_cast<CreditAccount*>(acc)->modifyCredit(newCredit);
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
}

void BankSystem::modifyShared(int id, bool shared) {
    if (!userMgr.isAdmin()) { logMgr.printFailure(); return; }
    Account* acc = accountMgr.findAccount(id);
    if (!acc) { logMgr.printFailure(); return; }
    acc->setShared(shared);
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
}

void BankSystem::changeUserPassword(const std::string &oldPassword, const std::string &newPassword) {
    User* user = userMgr.findUser(userMgr.getCurrentUserName());
    if (!user) { logMgr.printFailure(); return; }
    if (!user->changePassword(oldPassword, newPassword)) { logMgr.printFailure(); return; }
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
}

void BankSystem::changeAccountPassword(int id, int oldPassword, int newPassword) {
    if (!requireAccount(id, oldPassword)) return;
    accountMgr.findAccount(id)->changeAccountPassword(oldPassword, newPassword);
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
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
    if (accountMgr.deposit(id, amount, accountPassword)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::withdraw(int id, double amount, int accountPassword) {
    if (!requireAccount(id)) return;
    if (accountMgr.withdraw(id, amount, accountPassword)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::transfer(int srcId, int dstId, double amount, int srcAccountPassword) {
    if (!userMgr.ownsAccount(srcId)) { logMgr.printFailure(); return; }
    if (accountMgr.transfer(srcId, dstId, amount, srcAccountPassword)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::fixedDeposit(int id, double amount, int months, int accountPassword) {
    if (!requireAccount(id)) return;
    if (accountMgr.fixedDeposit(id, amount, months, accountPassword)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::fixedWithdraw(int id, double amount, int accountPassword) {
    if (!requireAccount(id)) return;
    if (accountMgr.fixedWithdraw(id, amount, accountPassword)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::consume(int id, double amount, int accountPassword) {
    if (!requireAccount(id)) return;
    if (accountMgr.consume(id, amount, accountPassword)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::cashAdvance(int id, double amount, int accountPassword) {
    if (!requireAccount(id)) return;
    if (accountMgr.cashAdvance(id, amount, accountPassword)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::showDate() const {
    if (!userMgr.isAdmin()) { logMgr.printFailure(); return; }
    accountMgr.showDate();
}

void BankSystem::addDays(int days) {
    if (!userMgr.isAdmin()) { logMgr.printFailure(); return; }
    if (accountMgr.addDays(days)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::setDate(int year, int month, int day) {
    if (!userMgr.isAdmin()) { logMgr.printFailure(); return; }
    if (accountMgr.setDate(year, month, day)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::createUser(const std::string &username, const std::string &password) {
    if (userMgr.createUser(username, password)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::deleteUser(const std::string &username) {
    if (userMgr.deleteUser(username)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::queryUser(const std::string &username) const {
    if (!userMgr.isAdmin()) { logMgr.printFailure(); return; }
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
    if (userMgr.switchUser(username, password)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::whoami() const {
    userMgr.whoami();
}

void BankSystem::showLog() const {
    if (!userMgr.isAdmin()) { logMgr.printFailure(); return; }
    logMgr.showLog();
}

void BankSystem::rollback(int n) {
    if (!userMgr.isAdmin()) { logMgr.printFailure(); return; }
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
    if (!userMgr.isAdmin()) { logMgr.printFailure(); return; }
    logMgr.saveLog(filename);
}

void BankSystem::resume(const std::string &filename) {
    if (!userMgr.isAdmin()) { logMgr.printFailure(); return; }
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
