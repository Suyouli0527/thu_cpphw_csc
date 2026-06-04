#include "banksystem.h"
#include "accountManager.h"
#include "userManager.h"
#include "logManager.h"
#include "command.h"
#include <fstream>
#include <sstream>
#include <algorithm>

BankSystem::BankSystem()
    : currentDate(1970, 1, 1), m_silent(false) {
    UserManager::reset(users, currentUserName);
}

BankSystem::~BankSystem() {
    AccountManager::clearAccounts(accounts);
}

bool BankSystem::requireAdmin() const {
    if (!isAdmin()) {
        LogManager::printFailure(m_silent);
        return false;
    }
    return true;
}

bool BankSystem::requireAccount(int id) const {
    if (!AccountManager::findAccount(accounts, id)) {
        LogManager::printFailure(m_silent);
        return false;
    }
    if (!UserManager::ownsAccount(users, currentUserName, id)) {
        LogManager::printFailure(m_silent);
        return false;
    }
    return true;
}

void BankSystem::logResult(bool ok) {
    if (ok) {
        LogManager::printSuccess(m_silent);
        LogManager::recordLog(logRecords, m_currentCommand);
    } else {
        LogManager::printFailure(m_silent);
    }
}

void BankSystem::openAccount(int id, char type, const std::string &accountName, double balance) {
    if (id <= 0) { LogManager::printFailure(m_silent); return; }
    if (AccountManager::findAccount(accounts, id) != nullptr) { LogManager::printFailure(m_silent); return; }
    if (type != 'S' && type != 'C') { LogManager::printFailure(m_silent); return; }
    if (accountName.empty()) { LogManager::printFailure(m_silent); return; }
    if (balance < 0) { LogManager::printFailure(m_silent); return; }

    Account* newAccount = nullptr;
    if (type == 'S') {
        newAccount = new SavingAccount(id, type, accountName, balance, currentDate);
    } else {
        newAccount = new CreditAccount(id, type, accountName, balance, currentDate);
    }
    if (!AccountManager::addAccount(accounts, newAccount)) {
        delete newAccount;
        LogManager::printFailure(m_silent);
        return;
    }
    UserManager::addAccountToUser(users, currentUserName, id);
    logResult(true);
}

void BankSystem::closeAccount(int id) {
    if (!requireAccount(id)) return;
    Account* acc = AccountManager::findAccount(accounts, id);
    if (acc->getBalance() != 0) { LogManager::printFailure(m_silent); return; }
    UserManager::removeAccountFromUser(users, currentUserName, id);
    AccountManager::removeAccount(accounts, id);
    logResult(true);
}

void BankSystem::modifyName(int id, const std::string &username) {
    if (!requireAccount(id)) return;
    if (username.empty()) { LogManager::printFailure(m_silent); return; }
    Account* acc = AccountManager::findAccount(accounts, id);
    acc->modifyName(username);
    logResult(true);
}

void BankSystem::modifyCredit(int id, double newCredit) {
    if (!requireAccount(id)) return;
    Account* acc = AccountManager::findAccount(accounts, id);
    if (acc->getType() != 'C') { LogManager::printFailure(m_silent); return; }
    if (newCredit < 0) { LogManager::printFailure(m_silent); return; }
    CreditAccount* creditAcc = static_cast<CreditAccount*>(acc);
    creditAcc->modifyCredit(newCredit);
    logResult(true);
}

void BankSystem::query(int id) const {
    if (!AccountManager::findAccount(accounts, id) ||
        !UserManager::ownsAccount(users, currentUserName, id)) {
        LogManager::printFailure(m_silent);
        return;
    }
    AccountManager::printAccountInfo(accounts, id);
}

void BankSystem::queryAllAccounts() const {
    User* user = UserManager::findUser(users, currentUserName);
    if (!user) { LogManager::printFailure(m_silent); return; }
    std::vector<int> ids = user->getAccountIDs();
    if (ids.empty()) { LogManager::printFailure(m_silent); return; }
    std::sort(ids.begin(), ids.end());
    for (int id : ids) {
        AccountManager::printAccountInfo(accounts, id);
    }
}

void BankSystem::deposit(int id, double amount) {
    if (!requireAccount(id)) return;
    logResult(AccountManager::deposit(accounts, currentDate, id, amount));
}

void BankSystem::withdraw(int id, double amount) {
    if (!requireAccount(id)) return;
    logResult(AccountManager::withdraw(accounts, currentDate, id, amount));
}

void BankSystem::transfer(int srcId, int dstId, double amount) {
    if (!UserManager::ownsAccount(users, currentUserName, srcId)) {
        LogManager::printFailure(m_silent);
        return;
    }
    logResult(AccountManager::transfer(accounts, currentDate, srcId, dstId, amount));
}

void BankSystem::showDate() const {
    Date::showDate(currentDate);
}

void BankSystem::addDays(int days) {
    if (!requireAdmin()) return;
    bool ok = Date::addDays(currentDate, days);
    if (ok) AccountManager::updateAllAccountsInterest(accounts, currentDate);
    logResult(ok);
}

void BankSystem::setDate(int year, int month, int day) {
    if (!requireAdmin()) return;
    bool ok = Date::setDate(currentDate, year, month, day);
    if (ok) AccountManager::updateAllAccountsInterest(accounts, currentDate);
    logResult(ok);
}

void BankSystem::createUser(const std::string &username) {
    logResult(UserManager::createUser(users, currentUserName, username));
}

void BankSystem::deleteUser(const std::string &username) {
    logResult(UserManager::deleteUser(users, currentUserName, username));
}

void BankSystem::queryUser(const std::string &username) const {
    if (!requireAdmin()) return;
    User* user = UserManager::findUser(users, username);
    if (!user) { LogManager::printFailure(m_silent); return; }
    std::vector<int> ids = user->getAccountIDs();
    if (ids.empty()) { LogManager::printFailure(m_silent); return; }
    std::sort(ids.begin(), ids.end());
    for (int id : ids) {
        AccountManager::printAccountInfo(accounts, id);
    }
}

void BankSystem::queryAllUser() const {
    if (!UserManager::queryAllUser(users, currentUserName)) {
        LogManager::printFailure(m_silent);
    }
}

void BankSystem::switchUser(const std::string &username) {
    logResult(UserManager::switchUser(users, currentUserName, username));
}

void BankSystem::whoami() const {
    UserManager::whoami(currentUserName);
}

void BankSystem::showLog() const {
    LogManager::showLog(logRecords, m_silent);
}

void BankSystem::rollback(int n) {
    if (n < 0 || n > static_cast<int>(logRecords.size())) {
        LogManager::printFailure(m_silent);
        return;
    }

    std::vector<std::string> cmdsToReplay(logRecords.begin(),
                                           logRecords.begin() + n);

    AccountManager::clearAccounts(accounts);
    UserManager::reset(users, currentUserName);
    currentDate = Date(1970, 1, 1);
    LogManager::clear(logRecords);

    replayCommands(cmdsToReplay);

    LogManager::printSuccess(m_silent);
}

void BankSystem::saveLog(const std::string &filename) const {
    LogManager::saveLog(logRecords, filename, m_silent);
}

void BankSystem::resume(const std::string &filename) {
    if (!LogManager::isInitialState(logRecords)) {
        LogManager::printFailure(m_silent);
        return;
    }
    std::ifstream file(filename);
    if (!file.is_open()) {
        LogManager::printFailure(m_silent);
        return;
    }
    std::vector<std::string> subCommands;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        size_t pos = line.find(' ');
        if (pos == std::string::npos) {
            LogManager::printFailure(m_silent);
            return;
        }
        subCommands.push_back(line.substr(pos + 1));
    }
    file.close();

    replayCommands(subCommands);

    LogManager::printSuccess(m_silent);
}

void BankSystem::replayCommands(const std::vector<std::string>& commands) {
    m_silent = true;
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
    m_silent = false;
}
