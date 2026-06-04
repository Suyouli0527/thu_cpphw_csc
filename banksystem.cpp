#include "banksystem.h"
#include "command.h"
#include <fstream>
#include <sstream>
#include <algorithm>

BankSystem::BankSystem()
    : accountMgr(), userMgr(), logMgr() {}

BankSystem::~BankSystem() {
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

void BankSystem::openAccount(int id, char type, const std::string &accountName, double balance) {
    if (id <= 0) { logMgr.printFailure(); return; }
    if (accountMgr.findAccount(id) != nullptr) { logMgr.printFailure(); return; }
    if (type != 'S' && type != 'C') { logMgr.printFailure(); return; }
    if (accountName.empty()) { logMgr.printFailure(); return; }
    if (balance < 0) { logMgr.printFailure(); return; }

    Account* newAccount = nullptr;
    if (type == 'S') {
        newAccount = new SavingAccount(id, type, accountName, balance, accountMgr.getCurrentDate());
    } else {
        newAccount = new CreditAccount(id, type, accountName, balance, accountMgr.getCurrentDate());
    }
    if (!accountMgr.addAccount(newAccount)) {
        delete newAccount;
        logMgr.printFailure();
        return;
    }
    userMgr.addAccountToUser(userMgr.getCurrentUserName(), id);
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
}

void BankSystem::closeAccount(int id) {
    if (!requireAccount(id)) return;
    Account* acc = accountMgr.findAccount(id);
    if (acc->getBalance() != 0) { logMgr.printFailure(); return; }
    userMgr.removeAccountFromUser(userMgr.getCurrentUserName(), id);
    accountMgr.removeAccount(id);
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
}

void BankSystem::modifyName(int id, const std::string &username) {
    if (!requireAccount(id)) return;
    if (username.empty()) { logMgr.printFailure(); return; }
    accountMgr.findAccount(id)->modifyName(username);
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
}

void BankSystem::modifyCredit(int id, double newCredit) {
    if (!requireAccount(id)) return;
    Account* acc = accountMgr.findAccount(id);
    if (acc->getType() != 'C') { logMgr.printFailure(); return; }
    if (newCredit < 0) { logMgr.printFailure(); return; }
    static_cast<CreditAccount*>(acc)->modifyCredit(newCredit);
    logMgr.printSuccess();
    logMgr.recordLog(logMgr.getCurrentCommand());
}

void BankSystem::query(int id) const {
    if (!accountMgr.findAccount(id) || !userMgr.ownsAccount(id)) {
        logMgr.printFailure();
        return;
    }
    accountMgr.printAccountInfo(id);
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

void BankSystem::deposit(int id, double amount) {
    if (!requireAccount(id)) return;
    if (accountMgr.deposit(id, amount)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::withdraw(int id, double amount) {
    if (!requireAccount(id)) return;
    if (accountMgr.withdraw(id, amount)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::transfer(int srcId, int dstId, double amount) {
    if (!userMgr.ownsAccount(srcId)) { logMgr.printFailure(); return; }
    if (accountMgr.transfer(srcId, dstId, amount)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::showDate() const {
    accountMgr.showDate();
}

void BankSystem::addDays(int days) {
    if (accountMgr.addDays(days)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::setDate(int year, int month, int day) {
    if (accountMgr.setDate(year, month, day)) {
        logMgr.printSuccess();
        logMgr.recordLog(logMgr.getCurrentCommand());
    } else {
        logMgr.printFailure();
    }
}

void BankSystem::createUser(const std::string &username) {
    if (userMgr.createUser(username)) {
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

void BankSystem::switchUser(const std::string &username) {
    if (userMgr.switchUser(username)) {
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
    logMgr.showLog();
}

void BankSystem::rollback(int n) {
    if (n < 0 || n > static_cast<int>(logMgr.getLogRecords().size())) {
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
    logMgr.saveLog(filename);
}

void BankSystem::resume(const std::string &filename) {
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
        size_t pos = line.find(' ');
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
    for (const auto &cmd : commands) {
        std::istringstream iss(cmd);
        std::string action;
        iss >> action;
        if (action == "QUERY" || action == "QUERYALL" || action == "QUERY_USER"
            || action == "QUERY_USERLIST" || action == "SHOW_DATE" || action == "WHOAMI"
            || action == "LOG" || action == "ROLLBACK" || action == "SAVE" || action == "RESUME") {
            continue;
        }
        Command::execute(*this, cmd);
    }
    logMgr.setSilent(false);
}
