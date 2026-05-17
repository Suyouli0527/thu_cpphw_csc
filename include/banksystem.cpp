#include "banksystem.h"
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
User* BankSystem::findUser(const std::string& name) const{
    for (const auto &user : users) {
        if (user.getUserName() == name) {
            return const_cast<User*>(&user);
        }
    }
    return nullptr;
}


Account* BankSystem::findAccount(int id) const{
    for (auto acc : accounts) {
        if (acc->getId() == id) {
            return acc;
        }
    }
    return nullptr;
}

void BankSystem::updateAllAccountsInterest(const Date &newDate) {
    for (auto acc : accounts) {
        acc->updateInterest(newDate);
    }
}

void BankSystem::removeAccount(int id) {
    findUser(currentUserName)->removeAccountID(id);
    for (auto it = accounts.begin(); it != accounts.end(); ++it) {
        if ((*it)->getId() == id) {
            delete *it;
            accounts.erase(it);
            break;
        }
    }
}

BankSystem::BankSystem():currentDate(1970,1,1),currentUserName("default"){
    users.emplace_back("admin",UserType::admin);
    users.emplace_back("default",UserType::normal);
}

void BankSystem::clearAccounts() {
    for (auto acc : accounts) {
        delete acc;
    }
    accounts.clear();
}

void BankSystem::printSuccess() const{
    if(!m_silent) Tools::printSuccess();
}

void BankSystem::printFailure() const{
    if(!m_silent) Tools::printFailure();
}

void BankSystem::openAccount(int id,char type,std::string accountName,double balance){
    if(findAccount(id)!=nullptr) {
        printFailure();
        return;
    }
    if(id<=0) {
        printFailure();
        return;
    }
    else if(type!='S'&&type!='C'&&type!='s'&&type!='c') {
        printFailure();
        return;
    }
    else if(accountName.empty()) {
        printFailure();
        return;
    }
    else if(balance<0) {
        printFailure();
        return;
    }
    else {
        Account* newAccount;
        Date openDate=currentDate;
        type = std::toupper(type);
        if(type=='S') newAccount=new SavingAccount(id,type,accountName,balance,openDate);
        else newAccount=new CreditAccount(id,type,accountName,balance,openDate);
        accounts.push_back(newAccount);
        findUser(currentUserName)->addAccountID(id);
        printSuccess();
        logRecords.push_back("OPEN "+std::to_string(id)+" "+std::string(1, type)+" "+accountName+" "+Tools::formatAmount(balance));
    }
}


void BankSystem::closeAccount(int id){
    Account* acc=findAccount(id);
    if(acc==nullptr || !ownsAccount(id)) {
        printFailure();
        return;
    }
    else if(Tools::formatAmount(acc->getBalance()) != "0.00") {
        printFailure();
        return;
    }
    else {
        removeAccount(id);
        printSuccess();
        logRecords.push_back("CLOSE "+std::to_string(id));
    }
}

void BankSystem::modifyName(int id,const std::string &username){
    Account* acc=findAccount(id);
    if(acc==nullptr || !ownsAccount(id)) {
        printFailure();
        return;
    }
    else if(username.empty()) {
        printFailure();
        return;
    }
    else {
        acc->modifyName(username);
        printSuccess();
        logRecords.push_back("MODIFY NAME "+std::to_string(id)+" "+username);
    }
}


void BankSystem::modifyCredit(int id,double newCredit){
    Account* acc=findAccount(id);
    if(acc==nullptr || !ownsAccount(id)) {
        printFailure();
        return;
    }
    else if(acc->getType()!='C') {
        printFailure();
        return;
    }
    else if(newCredit<0) {
        printFailure();
        return;
    }
    else {
        CreditAccount* creditAcc=static_cast<CreditAccount*>(acc);
        creditAcc->modifyCredit(newCredit);
        printSuccess();
        logRecords.push_back("MODIFY CREDIT "+std::to_string(id)+" "+Tools::formatAmount(newCredit));
    }
}

void BankSystem::query(int id) const{
    Account* acc=findAccount(id);
    if(acc==nullptr || !ownsAccount(id)) {
        printFailure();
        return;
    }
    printAccountInfo(id);
}

void BankSystem::queryAllAccounts() const{
    if(currentUserName.empty()) {
        printFailure();
        return;
    }
    const auto& accountIDs = findUser(currentUserName)->getAccountIDs();
    if(accountIDs.empty()) {
        printFailure();
        return;
    }

    
    std::vector<int> sortedAccounts = accountIDs;
    std::sort(sortedAccounts.begin(), sortedAccounts.end());
    for (int id : sortedAccounts) {
        query(id);
    }
}
void BankSystem::deposit(int id, double amount) {
    Account* acc = findAccount(id);
    if (acc==nullptr || !ownsAccount(id)) {
        printFailure();
        return;
    }
    if (acc->deposit(currentDate, amount)) {
        printSuccess();
        logRecords.push_back("DEPOSIT "+std::to_string(id)+" "+Tools::formatAmount(amount));
    }
    else {
        printFailure();
    }
}


void BankSystem::withdraw(int id, double amount){
    Account* acc=findAccount(id);
    if(acc==nullptr || !ownsAccount(id)) {
        printFailure();
        return;
    }
    if (acc->withdraw(currentDate, amount)) {
        printSuccess();
        logRecords.push_back("WITHDRAW "+std::to_string(id)+" "+Tools::formatAmount(amount));
    }
    else {
        printFailure();
    }
}

void BankSystem::transfer(int srcId, int dstId, double amount){
    if (srcId == dstId) { printFailure(); return; }
    Account* srcAcc=findAccount(srcId);
    Account* dstAcc=findAccount(dstId);
    if(srcAcc==nullptr||dstAcc==nullptr) {
        printFailure();
        return;
    }
    const auto& userAccountIDs = findUser(currentUserName)->getAccountIDs();
    bool belongsToCurrentUser = false;
    for (int id : userAccountIDs) {
        if (id == srcId) {
            belongsToCurrentUser = true;
            break;
        }
    }
    if (!belongsToCurrentUser) {
        printFailure();
        return;
    }
    if(!srcAcc->withdraw(currentDate, amount)) {
        printFailure();
        return;
    }
    dstAcc->deposit(currentDate, amount);
    printSuccess();
    logRecords.push_back("TRANSFER "+std::to_string(srcId)+" "+std::to_string(dstId)+" "+Tools::formatAmount(amount));
}

void BankSystem::showDate() const{
    currentDate.showDate();
}

void BankSystem::addDays(int days){
    if(currentDate.addDays(days)) {
        updateAllAccountsInterest(currentDate);
        printSuccess();
        logRecords.push_back("ADD_DAY "+std::to_string(days));
    }
    else {
        printFailure();
    }
}

void BankSystem::setDate(int year, int month, int day){
    if(currentDate.setDays(year,month,day)) {
        updateAllAccountsInterest(currentDate);
        printSuccess();
        logRecords.push_back("SET_DATE "+std::to_string(year)+" "+std::to_string(month)+" "+std::to_string(day));
    }
    else {
        printFailure();
    }
}


void BankSystem::createUser(const std::string &username){  
    if(!isAdmin()) {
        printFailure();
        return;
    }

    if(!islegalName(username)) {
        printFailure();
        return;
    }

    if(username=="admin") {
        printFailure();
        return;
    }


    if(findUser(username)) {
        printFailure();
        return;
    }


    else {
    users.emplace_back(username,UserType::normal);
    printSuccess();
    logRecords.push_back("CREATE_USER "+username);
}
}
void BankSystem::deleteUser(const std::string &username){
    if(!isAdmin()) {
        printFailure();
        return;
    }
    if(username=="admin") {
        printFailure();
        return;
    }
    if(findUser(username)==nullptr) {
        printFailure();
        return;
    }
    
    if(findUser(username)->getAccountIDs().size()>0) {
        printFailure();
        return;
    }
    else {
        for (int i = users.size() - 1; i >= 0; i--) {
            if (users[i].getUserName() == username) {
                users.erase(users.begin() + i);
                printSuccess();
                logRecords.push_back("DELETE_USER "+username);
                return;
            }
        }
    }
}


void BankSystem::queryUser(const std::string &username) const{
    if(!isAdmin()) {
        printFailure();
        return;
    }
    User* user=findUser(username);
    if(user==nullptr) {
        printFailure();
        return;
    }
    else if(user->getAccountIDs().empty()) {
        printFailure();
        return;
    }
    else {
        std::vector<int> ids = user->getAccountIDs();

        std::sort(ids.begin(), ids.end());

        for(auto id : ids) {
            printAccountInfo(id);
        }
    }

}

void BankSystem::queryAllUser() const{
    if(!isAdmin()) {
        printFailure();
        return;
    }
    if(users.empty()) {
        printFailure();
        return;
    }
    else{
        std::vector<User> sortedUsers = users;
        std::sort(sortedUsers.begin(), sortedUsers.end());
        for(auto user:sortedUsers) {
            std::cout<<"USER "<<user.getUserName()<<" "<<user.getAccountCount()<<std::endl;
        }
    }
}

void BankSystem::switchUser(const std::string &username){
    User* user=findUser(username);
    if(user==nullptr) {
        printFailure();
        return;
    }
    else {
        currentUserName=username;
        printSuccess();
        logRecords.push_back("SWITCH "+username);
    }
}

void BankSystem::whoami() const{
    if(currentUserName.empty()) {
        printFailure();
        return;
    }
    else {
        std::cout<<currentUserName<<std::endl;
    }
}

bool BankSystem::isAdmin() const{
    User* user = findUser(currentUserName);
    return user != nullptr && user->isAdmin();
}

bool BankSystem::islegalName(const std::string &name) const{
    if(name.empty()) return false;
    for(char c:name) {
        if(!((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9'))) return false;
    }
    return true;
}

bool BankSystem::ownsAccount(int id) const{
    const auto& myIDs = findUser(currentUserName)->getAccountIDs();
    for (int aid : myIDs) {
        if (aid == id) return true;
    }
    return false;
}

void BankSystem::printAccountInfo(int id) const{
    Account* acc = findAccount(id);
    std::cout<<acc->getId()<<" "
        <<acc->getType()<<" "
        <<acc->getName()<<" "
        <<Tools::formatAmount(acc->getBalance());
    if(acc->getType()=='C') {
        const CreditAccount* creditAcc=static_cast<const CreditAccount*>(acc);
        std::cout<<" "<<Tools::formatAmount(creditAcc->getCredit());
    }
    std::cout<<std::endl;
}

void BankSystem::showLog() const{
    if(logRecords.empty()) {
        printFailure();
        return;
    }

    else {
        for(int i=0;i<logRecords.size();i++) {
            std::cout<<i+1<<" "<<logRecords[i]<<std::endl;
        }
    }
    
}

void BankSystem::rollback(int n){
    if(n<0||n>logRecords.size()) {
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
    for (const auto& cmd : cmdsToReplay) {
        std::istringstream iss(cmd);
        std::string action;
        iss >> action;

        if (action == "OPEN") {
            int id; char type; std::string name; double balance;
            iss >> id >> type >> name >> balance;
            openAccount(id, type, name, balance);
        }
        else if (action == "CLOSE") {
            int id; iss >> id;
            closeAccount(id);
        }
        else if (action == "MODIFY") {
        std::string subAction;
        iss >> subAction;
        if (subAction == "NAME") {
        int id; std::string name;
        iss >> id >> name;
        modifyName(id, name);
    }
        else if (subAction == "CREDIT") {
        int id; double credit;
        iss >> id >> credit;
        modifyCredit(id, credit);
    }
}
        else if (action == "DEPOSIT") {
            int id; double amount;
            iss >> id >> amount;
            deposit(id, amount);
        }
        else if (action == "WITHDRAW") {
            int id; double amount;
            iss >> id >> amount;
            withdraw(id, amount);
        }
        else if (action == "TRANSFER") {
            int srcId, dstId; double amount;
            iss >> srcId >> dstId >> amount;
            transfer(srcId, dstId, amount);
        }
        else if (action == "SWITCH") {
            std::string username;
            iss >> username;
            switchUser(username);
        }
        else if (action == "CREATE_USER") {
            std::string username;
            iss >> username;
            createUser(username);
        }
        else if (action == "DELETE_USER") {
            std::string username;
            iss >> username;
            deleteUser(username);
        }
        else if (action == "ADD_DAY") {
            double days; iss >> days;
            if (days == static_cast<int>(days)) {
                addDays(static_cast<int>(days));
            } else {
                printFailure();
            }
        }
        else if (action == "SET_DATE") {
            int year, month, day;
            iss >> year >> month >> day;
            setDate(year, month, day);
        }
    }
    m_silent = false;

    printSuccess();
}

void BankSystem::SaveLog(const std::string &filename) const{
    std::ofstream file(filename);
    if (!file) {
        printFailure();
        return;
    }
     for ( int i = 0; i < logRecords.size(); i++) {
        file << (i + 1) << " " << logRecords[i] << std::endl;
    }
    printSuccess();

}


BankSystem::~BankSystem(){
    clearAccounts();
}