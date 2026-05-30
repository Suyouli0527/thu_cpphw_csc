#include "banksystem.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>

BankSystem::BankSystem() : currentDate(1970, 1, 1), currentUserName("default") {
    users.emplace_back("admin", UserType::admin, "admin");
    users.emplace_back("default", UserType::normal, "default");
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
    m_lastResult = true;
}

void BankSystem::printFailure() const {
    m_lastResult = false;
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
    Account* acc = findAccount(id);
    if (!acc) return false;
    for (const auto &o : acc->getOwners()) {
        if (o == currentUserName) return true;
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
        if (acc->getType() == 'S') {
            acc->updateInterest(newDate);
            SavingAccount* sa = static_cast<SavingAccount*>(acc);
            sa->updateFixedDeposits(newDate);
            sa->updateWealthProducts(newDate);
        } else if (acc->getType() == 'C') {
            static_cast<CreditAccount*>(acc)->updateCreditInterest(newDate);
        }
    }
}

void BankSystem::removeAccount(int id) {
    Account* acc = findAccount(id);
    if (acc) {
        for (const auto &ownerName : acc->getOwners()) {
            User* owner = findUser(ownerName);
            if (owner) owner->removeAccountID(id);
        }
    }
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
              << BankSystem::formatAmount(acc->getBalance());
    if (acc->isShared()) {
        std::cout << " 1";
        for (const auto &o : acc->getOwners()) {
            std::cout << " " << o;
        }
    } else {
        std::cout << " 0";
    }
    if (acc->getType() == 'C') {
        const CreditAccount* creditAcc = static_cast<const CreditAccount*>(acc);
        std::cout << " " << BankSystem::formatAmount(creditAcc->getCredit())
                  << " " << creditAcc->getRepaymentDay()
                  << " " << BankSystem::formatAmount(creditAcc->getCashAdvanceDebt())
                  << " " << BankSystem::formatAmount(creditAcc->getConsumeDebt());
    }
    if (acc->getType() == 'S') {
        const SavingAccount* savingAcc = static_cast<const SavingAccount*>(acc);
        std::cout << " " << savingAcc->getFixedDepositCount();
        for (const auto& fd : savingAcc->getFixedDeposits()) {
            std::cout << " " << fd.months << " "
                      << BankSystem::formatAmount(fd.principal) << " "
                      << fd.depositDate.getYear() << "-"
                      << fd.depositDate.getMonth() << "-"
                      << fd.depositDate.getDay() << " "
                      << fd.maturityDate.getYear() << "-"
                      << fd.maturityDate.getMonth() << "-"
                      << fd.maturityDate.getDay() << " "
                      << (fd.partiallyWithdrawn ? "1" : "0");
        }
        // 基金持仓
        std::cout << " " << savingAcc->getFundHoldingCount();
        for (const auto& fh : savingAcc->getFundHoldings()) {
            std::cout << " " << fh.fundIndex << " "
                      << BankSystem::formatAmount(fh.shares) << " "
                      << BankSystem::formatAmount(fh.navAtPurchase) << " "
                      << fh.purchaseDate.getYear() << "-"
                      << fh.purchaseDate.getMonth() << "-"
                      << fh.purchaseDate.getDay();
        }
        // 理财产品持仓
        std::cout << " " << savingAcc->getWealthHoldingCount();
        for (const auto& wp : savingAcc->getWealthHoldings()) {
            std::cout << " " << wp.productIndex << " "
                      << BankSystem::formatAmount(wp.principal) << " "
                      << wp.purchaseDate.getYear() << "-"
                      << wp.purchaseDate.getMonth() << "-"
                      << wp.purchaseDate.getDay() << " "
                      << wp.maturityDate.getYear() << "-"
                      << wp.maturityDate.getMonth() << "-"
                      << wp.maturityDate.getDay() << " "
                      << (wp.settled ? "1" : "0");
        }
    }
    std::cout << std::endl;
}

void BankSystem::openAccount(int id, char type, const std::string &accountName, double balance, int repaymentDay, int accountPassword, bool shared) {
    if (id <= 0) { printFailure(); return; }
    if (findAccount(id) != nullptr) { printFailure(); return; }
    if (type != 'S' && type != 'C') { printFailure(); return; }
    if (accountName.empty()) { printFailure(); return; }
    if (balance < 0) { printFailure(); return; }
    if (type == 'C' && (repaymentDay < 1 || repaymentDay > 28)) { printFailure(); return; }
    if (accountPassword < 100000 || accountPassword > 999999) { printFailure(); return; }

    Account* newAccount = nullptr;
    if (type == 'S') {
        newAccount = new SavingAccount(id, type, accountName, balance, currentDate, accountPassword, shared);
    } else {
        newAccount = new CreditAccount(id, type, accountName, balance, repaymentDay, currentDate, accountPassword, shared);
    }
    newAccount->addOwner(currentUserName);
    accounts.push_back(newAccount);
    User* user = findUser(currentUserName);
    if (user) user->addAccountID(id);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::closeAccount(int id, int accountPassword) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
    if (acc->getBalance() != 0) { printFailure(); return; }
    if (acc->getType() == 'S') {
        SavingAccount* sa = static_cast<SavingAccount*>(acc);
        if (sa->getFundHoldingCount() > 0 || sa->getWealthHoldingCount() > 0
            || sa->getFixedDepositCount() > 0) { printFailure(); return; }
    }
    removeAccount(id);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::addOwner(int id, const std::string &userName) {
    if (!isAdmin()) { printFailure(); return; }
    Account* acc = findAccount(id);
    if (!acc) { printFailure(); return; }
    if (!acc->isShared()) { printFailure(); return; }
    User* targetUser = findUser(userName);
    if (!targetUser) { printFailure(); return; }
    size_t before = acc->getOwners().size();
    acc->addOwner(userName);
    if (acc->getOwners().size() == before) { printFailure(); return; }
    targetUser->addAccountID(id);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::removeOwner(int id, const std::string &userName) {
    if (!isAdmin()) { printFailure(); return; }
    Account* acc = findAccount(id);
    if (!acc) { printFailure(); return; }
    if (!acc->isShared()) { printFailure(); return; }
    size_t before = acc->getOwners().size();
    acc->removeOwner(userName);
    if (acc->getOwners().size() == before) { printFailure(); return; }
    User* targetUser = findUser(userName);
    if (targetUser) targetUser->removeAccountID(id);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::modifyName(int id, const std::string &username, int accountPassword) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
    if (username.empty()) { printFailure(); return; }
    acc->modifyName(username);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::modifyCredit(int id, double newCredit, int accountPassword) {
    if (!isAdmin()) { printFailure(); return; }
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
    if (acc->getType() != 'C') { printFailure(); return; }
    if (newCredit < 0) { printFailure(); return; }
    CreditAccount* creditAcc = static_cast<CreditAccount*>(acc);
    creditAcc->modifyCredit(newCredit);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::modifyShared(int id, bool shared) {
    if (!isAdmin()) { printFailure(); return; }
    Account* acc = findAccount(id);
    if (!acc) { printFailure(); return; }
    acc->setShared(shared);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::changeUserPassword(const std::string &oldPassword, const std::string &newPassword) {
    User* user = findUser(currentUserName);
    if (!user) { printFailure(); return; }
    if (!user->changePassword(oldPassword, newPassword)) { printFailure(); return; }
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::changeAccountPassword(int id, int oldPassword, int newPassword) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->changeAccountPassword(oldPassword, newPassword)) { printFailure(); return; }
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::query(int id, int accountPassword) const {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
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


void BankSystem::deposit(int id, double amount, int accountPassword) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
    if (acc->deposit(currentDate, amount)) {
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

void BankSystem::withdraw(int id, double amount, int accountPassword) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
    if (acc->withdraw(currentDate, amount)) {
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

void BankSystem::transfer(int srcId, int dstId, double amount, int srcAccountPassword) {
    if (srcId == dstId) { printFailure(); return; }
    Account* srcAcc = findAccount(srcId);
    Account* dstAcc = findAccount(dstId);
    if (!srcAcc || !dstAcc) { printFailure(); return; }
    if (!ownsAccount(srcId)) { printFailure(); return; }
    if (!srcAcc->verifyAccountPassword(srcAccountPassword)) { printFailure(); return; }
    if (!srcAcc->withdraw(currentDate, amount)) { printFailure(); return; }
    dstAcc->deposit(currentDate, amount);
    printSuccess();
    logRecords.push_back(m_currentCommand);
}

void BankSystem::fixedDeposit(int id, double amount, int months, int accountPassword) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
    if (acc->getType() != 'S') { printFailure(); return; }
    SavingAccount* savingAcc = static_cast<SavingAccount*>(acc);
    if (savingAcc->fixedDeposit(currentDate, amount, months)) {
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

void BankSystem::fixedWithdraw(int id, double amount, int accountPassword) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
    if (acc->getType() != 'S') { printFailure(); return; }
    SavingAccount* savingAcc = static_cast<SavingAccount*>(acc);
    if (savingAcc->fixedWithdraw(currentDate, amount)) {
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

void BankSystem::consume(int id, double amount, int accountPassword) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
    if (acc->getType() != 'C') { printFailure(); return; }
    CreditAccount* creditAcc = static_cast<CreditAccount*>(acc);
    if (creditAcc->consume(currentDate, amount)) {
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

void BankSystem::cashAdvance(int id, double amount, int accountPassword) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
    if (acc->getType() != 'C') { printFailure(); return; }
    CreditAccount* creditAcc = static_cast<CreditAccount*>(acc);
    if (creditAcc->cashAdvance(currentDate, amount)) {
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

void BankSystem::buyFund(int id, int fundIndex, double amount, int accountPassword) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
    if (acc->getType() != 'S') { printFailure(); return; }
    SavingAccount* savingAcc = static_cast<SavingAccount*>(acc);
    if (savingAcc->buyFund(currentDate, fundIndex, amount)) {
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

void BankSystem::sellFund(int id, int holdingIndex, int accountPassword) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
    if (acc->getType() != 'S') { printFailure(); return; }
    SavingAccount* savingAcc = static_cast<SavingAccount*>(acc);
    if (savingAcc->sellFund(holdingIndex, currentDate)) {
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

void BankSystem::buyWealthProduct(int id, int productIndex, double amount, int accountPassword) {
    Account* acc = findAccount(id);
    if (!acc || !ownsAccount(id)) { printFailure(); return; }
    if (!acc->verifyAccountPassword(accountPassword)) { printFailure(); return; }
    if (acc->getType() != 'S') { printFailure(); return; }
    SavingAccount* savingAcc = static_cast<SavingAccount*>(acc);
    if (savingAcc->buyWealthProduct(currentDate, productIndex, amount)) {
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

// ==================== Date & Interest ====================

void BankSystem::showDate() const {
    if (!isAdmin()) { printFailure(); return; }
    currentDate.showDate();
}

void BankSystem::addDays(int days) {
    if (!isAdmin()) { printFailure(); return; }
    if (currentDate.addDays(days)) {
        updateAllAccountsInterest(currentDate);
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

void BankSystem::setDate(int year, int month, int day) {
    if (!isAdmin()) { printFailure(); return; }
    if (currentDate.setDate(year, month, day)) {
        updateAllAccountsInterest(currentDate);
        printSuccess();
        logRecords.push_back(m_currentCommand);
    } else {
        printFailure();
    }
}

// ==================== Multi-User Management ====================

void BankSystem::createUser(const std::string &username, const std::string &password) {
    if (!isAdmin()) { printFailure(); return; }
    if (!isLegalName(username)) { printFailure(); return; }
    if (username == "admin") { printFailure(); return; }
    if (password.empty()) { printFailure(); return; }
    if (findUser(username)) { printFailure(); return; }
    users.emplace_back(username, UserType::normal, password);
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

void BankSystem::switchUser(const std::string &username, const std::string &password) {
    User* user = findUser(username);
    if (!user) { printFailure(); return; }
    if (!user->verifyPassword(password)) { printFailure(); return; }
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
    if (!isAdmin()) { printFailure(); return; }
    if (logRecords.empty()) {
        printFailure();
        return;
    }
    for (size_t i = 0; i < logRecords.size(); i++) {
        std::cout << (i + 1) << " " << logRecords[i] << std::endl;
    }
}

void BankSystem::rollback(int n) {
    if (!isAdmin()) { printFailure(); return; }
    if (n < 0 || n > (int)logRecords.size()) {
        printFailure();
        return;
    }

    std::vector<std::string> cmdsToReplay(logRecords.begin(), logRecords.begin() + n);

    clearAccounts();
    users.clear();
    users.emplace_back("admin", UserType::admin, "admin");
    users.emplace_back("default", UserType::normal, "default");
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
            int repDay = 0, accPwd = 0, sharedInt = 0;
            if (type == 'C') { iss >> repDay; }
            iss >> accPwd >> sharedInt;
            openAccount(id, type, name, balance, repDay, accPwd, sharedInt != 0);
        } else if (action == "CLOSE") {
            int id, accPwd; iss >> id >> accPwd;
            closeAccount(id, accPwd);
        } else if (action == "MODIFY") {
            std::string subAction;
            iss >> subAction;
            if (subAction == "NAME") {
                int id, accPwd; std::string name;
                iss >> id >> name >> accPwd;
                modifyName(id, name, accPwd);
            } else if (subAction == "CREDIT") {
                int id, accPwd; double credit;
                iss >> id >> credit >> accPwd;
                modifyCredit(id, credit, accPwd);
            } else if (subAction == "SHARED") {
                int id; std::string sharedStr;
                iss >> id >> sharedStr;
                modifyShared(id, sharedStr == "ys");
            }
        } else if (action == "MODIFY_USERPASSWORD") {
            std::string oldPwd, newPwd;
            iss >> oldPwd >> newPwd;
            changeUserPassword(oldPwd, newPwd);
        } else if (action == "MODIFY_ACCOUNTPASSWORD") {
            int id, oldPwd, newPwd;
            iss >> id >> oldPwd >> newPwd;
            changeAccountPassword(id, oldPwd, newPwd);
        } else if (action == "ADD_OWNER") {
            int id; std::string userName;
            iss >> id >> userName;
            addOwner(id, userName);
        } else if (action == "REMOVE_OWNER") {
            int id; std::string userName;
            iss >> id >> userName;
            removeOwner(id, userName);
        } else if (action == "DEPOSIT") {
            int id, accPwd; double amount;
            iss >> id >> amount >> accPwd;
            deposit(id, amount, accPwd);
        } else if (action == "WITHDRAW") {
            int id, accPwd; double amount;
            iss >> id >> amount >> accPwd;
            withdraw(id, amount, accPwd);
        } else if (action == "TRANSFER") {
            int srcId, dstId, accPwd; double amount;
            iss >> srcId >> dstId >> amount >> accPwd;
            transfer(srcId, dstId, amount, accPwd);
        } else if (action == "FIXED_DEPOSIT") {
            int id, accPwd; double amount; int months;
            iss >> id >> amount >> months >> accPwd;
            fixedDeposit(id, amount, months, accPwd);
        } else if (action == "FIXED_WITHDRAW") {
            int id, accPwd; double amount;
            iss >> id >> amount >> accPwd;
            fixedWithdraw(id, amount, accPwd);
        } else if (action == "CONSUME") {
            int id, accPwd; double amount;
            iss >> id >> amount >> accPwd;
            consume(id, amount, accPwd);
        } else if (action == "CASH_ADVANCE") {
            int id, accPwd; double amount;
            iss >> id >> amount >> accPwd;
            cashAdvance(id, amount, accPwd);
        } else if (action == "BUY_FUND") {
            int id, accPwd, fundIndex; double amount;
            iss >> id >> fundIndex >> amount >> accPwd;
            buyFund(id, fundIndex, amount, accPwd);
        } else if (action == "SELL_FUND") {
            int id, accPwd, holdingIndex;
            iss >> id >> holdingIndex >> accPwd;
            sellFund(id, holdingIndex, accPwd);
        } else if (action == "BUY_WP") {
            int id, accPwd, productIndex; double amount;
            iss >> id >> productIndex >> amount >> accPwd;
            buyWealthProduct(id, productIndex, amount, accPwd);
        } else if (action == "ADD_DAY") {
            int days; iss >> days;
            addDays(days);
        } else if (action == "SET_DATE") {
            int y, m, d; iss >> y >> m >> d;
            setDate(y, m, d);
        } else if (action == "SWITCH") {
            std::string username, pwd; iss >> username >> pwd;
            switchUser(username, pwd);
        } else if (action == "CREATE_USER") {
            std::string username, pwd; iss >> username >> pwd;
            createUser(username, pwd);
        } else if (action == "DELETE_USER") {
            std::string username; iss >> username;
            deleteUser(username);
        }
    }
    m_silent = false;

    printSuccess();
}

void BankSystem::saveLog(const std::string &filename) const {
    if (!isAdmin()) { printFailure(); return; }
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
    if (!isAdmin()) { printFailure(); return; }
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
            int repDay = 0, accPwd = 0, sharedInt = 0;
            if (type == 'C') { iss >> repDay; }
            iss >> accPwd >> sharedInt;
            openAccount(id, type, name, balance, repDay, accPwd, sharedInt != 0);
        } else if (action == "CLOSE") {
            int id, accPwd; iss >> id >> accPwd;
            closeAccount(id, accPwd);
        } else if (action == "MODIFY") {
            std::string subAction; iss >> subAction;
            if (subAction == "NAME") {
                int id, accPwd; std::string name;
                iss >> id >> name >> accPwd;
                modifyName(id, name, accPwd);
            } else if (subAction == "CREDIT") {
                int id, accPwd; double credit;
                iss >> id >> credit >> accPwd;
                modifyCredit(id, credit, accPwd);
            } else if (subAction == "SHARED") {
                int id; std::string sharedStr;
                iss >> id >> sharedStr;
                modifyShared(id, sharedStr == "ys");
            }
        } else if (action == "MODIFY_USERPASSWORD") {
            std::string oldPwd, newPwd;
            iss >> oldPwd >> newPwd;
            changeUserPassword(oldPwd, newPwd);
        } else if (action == "MODIFY_ACCOUNTPASSWORD") {
            int id, oldPwd, newPwd;
            iss >> id >> oldPwd >> newPwd;
            changeAccountPassword(id, oldPwd, newPwd);
        } else if (action == "ADD_OWNER") {
            int id; std::string userName;
            iss >> id >> userName;
            addOwner(id, userName);
        } else if (action == "REMOVE_OWNER") {
            int id; std::string userName;
            iss >> id >> userName;
            removeOwner(id, userName);
        } else if (action == "DEPOSIT") {
            int id, accPwd; double amount;
            iss >> id >> amount >> accPwd;
            deposit(id, amount, accPwd);
        } else if (action == "WITHDRAW") {
            int id, accPwd; double amount;
            iss >> id >> amount >> accPwd;
            withdraw(id, amount, accPwd);
        } else if (action == "TRANSFER") {
            int srcId, dstId, accPwd; double amount;
            iss >> srcId >> dstId >> amount >> accPwd;
            transfer(srcId, dstId, amount, accPwd);
        } else if (action == "FIXED_DEPOSIT") {
            int id, accPwd; double amount; int months;
            iss >> id >> amount >> months >> accPwd;
            fixedDeposit(id, amount, months, accPwd);
        } else if (action == "FIXED_WITHDRAW") {
            int id, accPwd; double amount;
            iss >> id >> amount >> accPwd;
            fixedWithdraw(id, amount, accPwd);
        } else if (action == "CONSUME") {
            int id, accPwd; double amount;
            iss >> id >> amount >> accPwd;
            consume(id, amount, accPwd);
        } else if (action == "CASH_ADVANCE") {
            int id, accPwd; double amount;
            iss >> id >> amount >> accPwd;
            cashAdvance(id, amount, accPwd);
        } else if (action == "BUY_FUND") {
            int id, accPwd, fundIndex; double amount;
            iss >> id >> fundIndex >> amount >> accPwd;
            buyFund(id, fundIndex, amount, accPwd);
        } else if (action == "SELL_FUND") {
            int id, accPwd, holdingIndex;
            iss >> id >> holdingIndex >> accPwd;
            sellFund(id, holdingIndex, accPwd);
        } else if (action == "BUY_WP") {
            int id, accPwd, productIndex; double amount;
            iss >> id >> productIndex >> amount >> accPwd;
            buyWealthProduct(id, productIndex, amount, accPwd);
        } else if (action == "ADD_DAY") {
            int y, m, d; iss >> y >> m >> d;
            setDate(y, m, d);
        } else if (action == "SWITCH") {
            std::string username, pwd; iss >> username >> pwd;
            switchUser(username, pwd);
        } else if (action == "CREATE_USER") {
            std::string username, pwd; iss >> username >> pwd;
            createUser(username, pwd);
        } else if (action == "DELETE_USER") {
            std::string username; iss >> username;
            deleteUser(username);
        }
    }
    m_silent = false;

    printSuccess();
}
