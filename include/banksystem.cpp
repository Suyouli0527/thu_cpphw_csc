#include "banksystem.h"

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
    currentUser->removeAccountID(id);
    for (auto it = accounts.begin(); it != accounts.end(); ++it) {
        if ((*it)->getId() == id) {
            delete *it;
            accounts.erase(it);
            break;
        }
    }
}

BankSystem::BankSystem():currentDate(1970,1,1),currentUser(nullptr){
    users.emplace_back("admin",UserType::admin);
    users.emplace_back("default",UserType::normal);
    currentUser = findUser("default");
}

void BankSystem::clearAccounts() {
    for (auto acc : accounts) {
        delete acc;
    }
    accounts.clear();
}

void BankSystem::openAccount(int id,char type,std::string accountName,double balance){
    if(findAccount(id)!=nullptr) {
        Tools::printFailure();
        return;
    }
    else if(type!='S'&&type!='C'&&type!='s'&&type!='c') {
        Tools::printFailure();
        return;
    }
    else if(accountName.empty()) {
        Tools::printFailure();
        return;
    }
    else if(balance<0) {
        Tools::printFailure();
        return;
    }
    else {
        Account* newAccount;
        Date openDate=currentDate;
        if(type=='S'||type=='s') newAccount=new SavingAccount(id,type,accountName,balance,openDate);
        else newAccount=new CreditAccount(id,type,accountName,balance,openDate);
        accounts.push_back(newAccount);
        currentUser->addAccountID(id);
        Tools::printSuccess();
    }
}


void BankSystem::closeAccount(int id){
    Account* acc=findAccount(id);
    if(acc==nullptr) {
        Tools::printFailure();
        return;
    }
    else if(acc->getBalance()!=0) {
        Tools::printFailure();
        return;
    }
    else {
        removeAccount(id);
        Tools::printSuccess();
    }
}

void BankSystem::modifyName(int id,const std::string &username){
    Account* acc=findAccount(id);
    if(acc==nullptr) {
        Tools::printFailure();
        return;
    }
    else if(username.empty()) {
        Tools::printFailure();
        return;
    }
    else {
        acc->modifyName(username);
        Tools::printSuccess();
    }
}


void BankSystem::modifyCredit(int id,double newCredit){
    Account* acc=findAccount(id);
    if(acc==nullptr) {
        Tools::printFailure();
        return;
    }
    else if(acc->getType()!='c') {
        Tools::printFailure();
        return;
    }
    else if(newCredit<0) {
        Tools::printFailure();
        return;
    }
    else {
        CreditAccount* creditAcc=static_cast<CreditAccount*>(acc);
        creditAcc->modifyCredit(newCredit);
        Tools::printSuccess();
    }
}

void BankSystem::query(int id) const{
    Account* acc=findAccount(id);
    if(acc==nullptr) {
        Tools::printFailure();
        return;
    }
    std::cout<<acc->getId()<<" "
        <<acc->getType()<<" "
        <<acc->getName()<<" "
        <<acc->getBalance();
    if(acc->getType()=='c') {
        const CreditAccount* creditAcc=static_cast<const CreditAccount*>(acc);
        std::cout<<" "<<creditAcc->getCredit();
        }
    std::cout<<std::endl;
    }

void BankSystem::queryAllAccounts() const{
    if(currentUser==nullptr) {
        Tools::printFailure();
        return;
    }
    const auto& accountIDs = currentUser->getAccountIDs();
    if(accountIDs.empty()) {
        Tools::printFailure();
        return;
    }

    
    std::vector<int> sortedAccounts = accountIDs;
    std::sort(sortedAccounts.begin(), sortedAccounts.end());
    for (int id : accountIDs) {
        query(id);
    }
}
void BankSystem::deposit(int id, double amount) {
    Account* acc = findAccount(id);
    if (acc==nullptr) {
        Tools::printFailure();
        return;
    }
    if (acc->deposit(currentDate, amount)) {
        Tools::printSuccess();
    }
    else {
        Tools::printFailure();
    }
}


void BankSystem::withdraw(int id, double amount){
    Account* acc=findAccount(id);
    if(acc==nullptr) {
        Tools::printFailure();
        return;
    }
    if (acc->withdraw(currentDate, amount)) {
        Tools::printSuccess();
    }
    else {
        Tools::printFailure();
    }
}

void BankSystem::transfer(int srcId, int dstId, double amount){
    if (srcId == dstId) { Tools::printFailure(); return; }
    Account* srcAcc=findAccount(srcId);
    Account* dstAcc=findAccount(dstId);
    if(srcAcc==nullptr||dstAcc==nullptr) {
        Tools::printFailure();
        return;
    }
    const auto& userAccountIDs = currentUser->getAccountIDs();
    bool belongsToCurrentUser = false;
    for (int id : userAccountIDs) {
        if (id == srcId) {
            belongsToCurrentUser = true;
            break;
        }
    }
    if (!belongsToCurrentUser) {
        Tools::printFailure();
        return;
    }
    if(!srcAcc->withdraw(currentDate, amount)) {
        Tools::printFailure();
        return;
    }
    dstAcc->deposit(currentDate, amount);
    Tools::printSuccess();
    
}

void BankSystem::showDate() const{
    currentDate.showDate();
}

void BankSystem::addDays(int days){
    if(currentDate.addDays(days)) {
        updateAllAccountsInterest(currentDate);
        Tools::printSuccess();
    }
    else {
        Tools::printFailure();
    }
}

void BankSystem::setDate(int year, int month, int day){
    if(currentDate.setDays(year,month,day)) {
        updateAllAccountsInterest(currentDate);
    }
}


void BankSystem::createUser(const std::string &username){  
    if(!isAdmin()) {
        Tools::printFailure();
        return;
    }
    if(username=="admin") {
        Tools::printFailure();
        return;
    }
    if(findUser(username)) {
        Tools::printFailure();
        return;
    }


    else if(username.empty()) {
        Tools::printFailure();
        return;
    }

    else {
    users.emplace_back(username,UserType::normal);
    Tools::printSuccess();
}
}
void BankSystem::deleteUser(const std::string &username){
    if(username=="admin") {
        Tools::printFailure();
        return;
    }
    else if(findUser(username)==nullptr) {
        Tools::printFailure();
        return;
    }
    else if(!isAdmin()) {
        Tools::printFailure();
        return;
    }
    else if(findUser(username)->getAccountIDs().size()>0) {
        Tools::printFailure();
        return;
    }

    for (int i = users.size() - 1; i >= 0; i--) {
    if (users[i].getUserName() == username) {
        users.erase(users.begin() + i);
    }
}
    Tools::printSuccess();
}

void BankSystem::queryUser(const std::string &username) const{
    if(!isAdmin()) {
        Tools::printFailure();
        return;
    }
    User* user=findUser(username);
    if(user==nullptr) {
        Tools::printFailure();
        return;
    }
    else if(user->getAccountIDs().empty()) {
        Tools::printFailure();
        return;
    }
    else {
        std::vector<int> ids = user->getAccountIDs();
        
        std::sort(ids.begin(), ids.end());
        
        for(auto id : ids) {
            query(id);
        }
    }

}

void BankSystem::queryAllUser() const{
    if(!isAdmin()) {
        Tools::printFailure();
        return;
    }
    if(users.empty()) {
        Tools::printFailure();
        return;
    }
    else if(currentUser->getUserType()!=UserType::admin) {
        Tools::printFailure();
        return;
    }
    else{
        std::vector<User> sortedUsers = users;
        std::sort(sortedUsers.begin(), sortedUsers.end());
        for(auto user:sortedUsers) {
            std::cout<<user.getUserName()<<" "<<user.getAccountCount()<<std::endl;
        }
    }
}

void BankSystem::switchUser(const std::string &username){
    User* user=findUser(username);
    if(user==nullptr) {
        Tools::printFailure();
        return;
    }
    else {
        currentUser=user;
        Tools::printSuccess();
    }
}

void BankSystem::whoami() const{
    if(currentUser==nullptr) {
        Tools::printFailure();
        return;
    }
    else {
        std::cout<<currentUser->getUserName()<<std::endl;
    }
}


BankSystem::~BankSystem(){
    clearAccounts();
}