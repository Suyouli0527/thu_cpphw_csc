#include "banksystem.h"
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>

BankSystem::BankSystem() : currentDate(1970, 1, 1), currentUserName("default") {
    users.emplace_back("admin", UserType::admin);
    users.emplace_back("default", UserType::normal);
}

BankSystem::~BankSystem() {
    clearAccounts();
}

void BankSystem::clearAccounts() {
    for (auto acc : accounts) {
        delete acc;
    }
    accounts.clear();
}

void BankSystem::printSuccess() const {
    if (!m_silent) Tools::printSuccess();
}

void BankSystem::printFailure() const {
    if (!m_silent) Tools::printFailure();
}

User* BankSystem::findUser(const std::string &name) const {
    for (const auto &user : users) {
        if (user.getUserName() == name) {
            return const_cast<User*>(&user);
        }
    }
    return nullptr;
}

Account* BankSystem::findAccount(int id) const {
    for (auto acc : accounts) {
        if (acc->getId() == id) {
            return acc;
        }
    }
    return nullptr;
}

bool BankSystem::ownsAccount(int id) const {
    User* user = findUser(currentUserName);
    if (!user) return false;
    const auto &ids = user->getAccountIDs();
    for (int aid : ids) {
        if (aid == id) return true;
    }
    return false;
}

bool BankSystem::isAdmin() const {
    User* user = findUser(currentUserName);
    return user != nullptr && user->isAdmin();
}

bool BankSystem::isLegalName(const std::string &name) const {
    if (name.empty()) return false;
    for (char c : name) {
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {
            return false;
        }
    }
    return true;
}

void BankSystem::updateAllAccountsInterest(const Date &newDate) {
    for (auto acc : accounts) {
        acc->updateInterest(newDate);
    }
}

void BankSystem::removeAccount(int id) {
    User* user = findUser(currentUserName);
    if (user) user->removeAccountID(id);
    for (auto it = accounts.begin(); it != accounts.end(); ++it) {
        if ((*it)->getId() == id) {
            delete *it;
            accounts.erase(it);
            break;
        }
    }
}

void BankSystem::printAccountInfo(int id) const {
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

// ==================== Account Management ====================

void BankSystem::openAccount(int id, char type, const std::string &accountName, double balance) {
    if (id <= 0) { printFailure(); return; }
    if (findAccount(id) != nullptr) { printFailure(); return; }
    type = std::toupper(type);
    if (type != 'S' && type != 'C') { printFailure(); return; }
    if (accountName.empty()) { printFailure(); return; }
    if (balance < 0) { printFailure(); return; }

    Account* newAccount = nullptr;
    if (type == 'S') {
        newAccount = new SavingAccount(id, type, accountName, balance, currentDate);
    } else {
        newAccount = new CreditAccount(id, type, accountName, balance, currentDate);
    }
    accounts.push_back(newAccount);
    User* user = findUser(currentUserName);
    if (user) user->addAccountID(id);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::closeAccount(int id) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (acc->getBalance() != 0) { printFailure(); return; }
    removeAccount(id);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::modifyName(int id, const std::string &username) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (username.empty()) { printFailure(); return; }
    acc->modifyName(username);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::modifyCredit(int id, double newCredit) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (acc->getType() != 'C') { printFailure(); return; }
    if (newCredit < 0) { printFailure(); return; }
    CreditAccount* creditAcc = static_cast<CreditAccount*>(acc);
    creditAcc->modifyCredit(newCredit);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::query(int id) const {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    printAccountInfo(id);
}

void BankSystem::queryAllAccounts() const {
    User* user = findUser(currentUserName);
    if (!user) { printFailure(); return; }
    std::vector<int> ids = user->getAccountIDs();
    if (ids.empty()) { printFailure(); return; }
    std::sort(ids.begin(), ids.end());
    for (int id : ids) {
        printAccountInfo(id);
    }
}

// ==================== Financial Transactions ====================

void BankSystem::deposit(int id, double amount) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (acc->deposit(currentDate, amount)) {
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

void BankSystem::withdraw(int id, double amount) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (acc->withdraw(currentDate, amount)) {
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

void BankSystem::transfer(int srcId, int dstId, double amount) {
    if (srcId == dstId) { printFailure(); return; }
    Account* srcAcc = findAccount(srcId);
    Account* dstAcc = findAccount(dstId);
    if (!srcAcc || !dstAcc) { printFailure(); return; }
    if (!ownsAccount(srcId)) { printFailure(); return; }
    if (!srcAcc->withdraw(currentDate, amount)) { printFailure(); return; }
    dstAcc->deposit(currentDate, amount);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

// ==================== Date & Interest ====================

void BankSystem::showDate() const {
    currentDate.showDate();
}

void BankSystem::addDays(int days) {
    if (currentDate.addDays(days)) {
        updateAllAccountsInterest(currentDate);
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

void BankSystem::setDate(int year, int month, int day) {
    if (currentDate.setDate(year, month, day)) {
        updateAllAccountsInterest(currentDate);
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

// ==================== Multi-User Management ====================

void BankSystem::createUser(const std::string &username) {
    if (!isAdmin()) { printFailure(); return; }
    if (!isLegalName(username)) { printFailure(); return; }
    if (username == "admin") { printFailure(); return; }
    if (findUser(username)) { printFailure(); return; }
    users.emplace_back(username, UserType::normal);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::deleteUser(const std::string &username) {
    if (!isAdmin()) { printFailure(); return; }
    if (username == "admin") { printFailure(); return; }
    if (username == currentUserName) { printFailure(); return; }
    User* user = findUser(username);
    if (!user) { printFailure(); return; }
    if (user->getAccountCount() > 0) { printFailure(); return; }
    for (size_t i = 0; i < users.size(); i++) {
        if (users[i].getUserName() == username) {
            users.erase(users.begin() + i);
            printSuccess();
            logRecords.push_back(m_currentCommand);
            return;
        }
    }
}

void BankSystem::queryUser(const std::string &username) const {
    if (!isAdmin()) { printFailure(); return; }
    User* user = findUser(username);
    if (!user) { printFailure(); return; }
    std::vector<int> ids = user->getAccountIDs();
    if (ids.empty()) { printFailure(); return; }
    std::sort(ids.begin(), ids.end());
    for (int id : ids) {
        printAccountInfo(id);
    }
}

void BankSystem::queryAllUser() const {
    if (!isAdmin()) { printFailure(); return; }
    if (users.empty()) { printFailure(); return; }
    std::vector<User> sortedUsers = users;
    std::sort(sortedUsers.begin(), sortedUsers.end());
    for (const auto &user : sortedUsers) {
        std::cout << "USER " << user.getUserName() << " " << user.getAccountCount() << std::endl;
    }
}

void BankSystem::switchUser(const std::string &username) {
    User* user = findUser(username);
    if (!user) { printFailure(); return; }
    currentUserName = username;
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::whoami() const {
    if (currentUserName.empty()) { printFailure(); return; }
    std::cout << currentUserName << std::endl;
}

// ==================== Log & Rollback ====================

void BankSystem::showLog() const {
    if (logRecords.empty()) {
        printFailure();
        return;
    }
    for (size_t i = 0; i < logRecords.size(); i++) {
        std::cout << (i + 1) << " " << logRecords[i] << std::endl;
    }
}

void BankSystem::rollback(int n) {
    if (n < 0 || n > (int)logRecords.size()) {
        printFailure();
        return;
    }

    std::vector<std::string> cmdsToReplay(logRecords.begin(), logRecords.begin() + n);

    clearAccounts();
    users.clear();
    users.emplace_back("admin", UserType::admin);
    users.emplace_back("default", UserType::normal);
    currentDate = Date(1970, 1, 1);
    currentUserName = "default";
    logRecords.clear();

    m_silent = true;
    for (const auto &cmd : cmdsToReplay) {
        m_currentCommand = cmd;
        std::istringstream iss(cmd);
        std::string action;
        iss >> action;

        if (action == "OPEN") {
            int id; char type; std::string name; double balance;
            iss >> id >> type >> name >> balance;
            openAccount(id, type, name, balance);
        } else if (action == "CLOSE") {
            int id; iss >> id;
            closeAccount(id);
        } else if (action == "MODIFY") {
            std::string subAction;
            iss >> subAction;
            if (subAction == "NAME") {
                int id; std::string name;
                iss >> id >> name;
                modifyName(id, name);
            } else if (subAction == "CREDIT") {
                int id; double credit;
                iss >> id >> credit;
                modifyCredit(id, credit);
            }
        } else if (action == "DEPOSIT") {
            int id; double amount;
            iss >> id >> amount;
            deposit(id, amount);
        } else if (action == "WITHDRAW") {
            int id; double amount;
            iss >> id >> amount;
            withdraw(id, amount);
        } else if (action == "TRANSFER") {
            int srcId, dstId; double amount;
            iss >> srcId >> dstId >> amount;
            transfer(srcId, dstId, amount);
        } else if (action == "ADD_DAY") {
            int days; iss >> days;
            addDays(days);
        } else if (action == "SET_DATE") {
            int y, m, d; iss >> y >> m >> d;
            setDate(y, m, d);
        } else if (action == "SWITCH") {
            std::string username; iss >> username;
            switchUser(username);
        } else if (action == "CREATE_USER") {
            std::string username; iss >> username;
            createUser(username);
        } else if (action == "DELETE_USER") {
            std::string username; iss >> username;
            deleteUser(username);
        }
    }
    m_silent = false;

    printSuccess();
}

// ==================== File Persistence ====================

void BankSystem::saveLog(const std::string &filename) const {
    std::ofstream file(filename);
    if (!file) {
        printFailure();
        return;
    }
    for (size_t i = 0; i < logRecords.size(); i++) {
        file << (i + 1) << " " << logRecords[i] << std::endl;
    }
    printSuccess();
}

void BankSystem::resume(const std::string &filename) {
    if (!logRecords.empty()) {
        printFailure();
        return;
    }
    std::ifstream file(filename);
    if (!file.is_open()) {
        printFailure();
        return;
    }
    std::vector<std::string> subCommands;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        size_t pos = line.find(' ');
        if (pos == std::string::npos) {
            printFailure();
            return;
        }
        subCommands.push_back(line.substr(pos + 1));
    }
    file.close();

    m_silent = true;
    for (const auto &cmd : subCommands) {
        m_currentCommand = cmd;
        std::istringstream iss(cmd);
        std::string action;
        iss >> action;

        if (action == "OPEN") {
            int id; char type; std::string name; double balance;
            iss >> id >> type >> name >> balance;
            openAccount(id, type, name, balance);
        } else if (action == "CLOSE") {
            int id; iss >> id;
            closeAccount(id);
        } else if (action == "MODIFY") {
            std::string subAction; iss >> subAction;
            if (subAction == "NAME") {
                int id; std::string name;
                iss >> id >> name;
                modifyName(id, name);
            } else if (subAction == "CREDIT") {
                int id; double credit;
                iss >> id >> credit;
                modifyCredit(id, credit);
            }
        } else if (action == "DEPOSIT") {
            int id; double amount;
            iss >> id >> amount;
            deposit(id, amount);
        } else if (action == "WITHDRAW") {
            int id; double amount;
            iss >> id >> amount;
            withdraw(id, amount);
        } else if (action == "TRANSFER") {
            int srcId, dstId; double amount;
            iss >> srcId >> dstId >> amount;
            transfer(srcId, dstId, amount);
        } else if (action == "ADD_DAY") {
            int days; iss >> days;
            addDays(days);
        } else if (action == "SET_DATE") {
            int y, m, d; iss >> y >> m >> d;
            setDate(y, m, d);
        } else if (action == "SWITCH") {
            std::string username; iss >> username;
            switchUser(username);
        } else if (action == "CREATE_USER") {
            std::string username; iss >> username;
            createUser(username);
        } else if (action == "DELETE_USER") {
            std::string username; iss >> username;
            deleteUser(username);
        }
    }
    m_silent = false;

    printSuccess();
}
