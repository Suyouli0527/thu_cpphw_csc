#include "banksystem.h"
#include <iomanip>
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
        logRecords.push_back("OPEN "+std::to_string(id)+" "+type+" "+accountName+" "+std::to_string(balance));
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
        logRecords.push_back("CLOSE "+std::to_string(id));
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
        logRecords.push_back("MODIFY_NAME "+std::to_string(id)+" "+username);
    }
}


void BankSystem::modifyCredit(int id,double newCredit){
    Account* acc=findAccount(id);
    if(acc==nullptr) {
        Tools::printFailure();
        return;
    }
    else if(acc->getType()!='C') {
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
        logRecords.push_back("MODIFY_CREDIT "+std::to_string(id)+" "+std::to_string(newCredit));
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
        <<std::fixed<<std::setprecision(2)<<acc->getBalance();
    if(acc->getType()=='C') {
        const CreditAccount* creditAcc=static_cast<const CreditAccount*>(acc);
        std::cout<<" "<<std::fixed<<std::setprecision(2)<<creditAcc->getCredit();
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
    for (int id : sortedAccounts) {
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
        logRecords.push_back("DEPOSIT "+std::to_string(id)+" "+std::to_string(amount));
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
        logRecords.push_back("WITHDRAW "+std::to_string(id)+" "+std::to_string(amount));
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
    logRecords.push_back("TRANSFER "+std::to_string(srcId)+" "+std::to_string(dstId)+" "+std::to_string(amount));
}

void BankSystem::showDate() const{
    currentDate.showDate();
}

void BankSystem::addDays(int days){
    if(currentDate.addDays(days)) {
        updateAllAccountsInterest(currentDate);
        Tools::printSuccess();
        logRecords.push_back("ADD_DAYS "+std::to_string(days));
    }
    else {
        Tools::printFailure();
    }
}

void BankSystem::setDate(int year, int month, int day){
    if(currentDate.setDays(year,month,day)) {
        updateAllAccountsInterest(currentDate);
        logRecords.push_back("SET_DATE "+std::to_string(year)+" "+std::to_string(month)+" "+std::to_string(day));
    }
}


void BankSystem::createUser(const std::string &username){  
    if(!isAdmin()) {
        Tools::printFailure();
        return;
    }

    if(!islegalName(username)) {
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


    else {
    users.emplace_back(username,UserType::normal);
    Tools::printSuccess();
    logRecords.push_back("CREATE_USER "+username);
}
}
void BankSystem::deleteUser(const std::string &username){
    if(!isAdmin()) {
        Tools::printFailure();
        return;
    }
    if(username=="admin") {
        Tools::printFailure();
        return;
    }
    if(findUser(username)==nullptr) {
        Tools::printFailure();
        return;
    }
    
    if(findUser(username)->getAccountIDs().size()>0) {
        Tools::printFailure();
        return;
    }
    else {
        for (int i = users.size() - 1; i >= 0; i--) {
            if (users[i].getUserName() == username) {
                users.erase(users.begin() + i);
                Tools::printSuccess();
                logRecords.push_back("DELETE_USER "+username);
                return;
            }
        }
    }
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
            std::cout<<"USER "<<user.getUserName()<<" "<<user.getAccountCount()<<std::endl;
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
        logRecords.push_back("SWITCH "+username);
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

bool BankSystem::islegalName(const std::string &name) const{
    if(name.empty()) return false;
    for(char c:name) {
        if(!((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9'))) return false;
    }
    return true;
}

void BankSystem::showLog() const{
    if(logRecords.empty()) {
        Tools::printFailure();
        return;
    }

    else {
        for(int i=0;i<logRecords.size();i++) {
            std::cout<<i+1<<" "<<logRecords[i]<<std::endl;
        }
    }
    
}

void BankSystem::rollback(int n){
    if(n<=0||n>logRecords.size()) {
        Tools::printFailure();
        return;
    }
    else {
        logRecords.erase(logRecords.end()-n,logRecords.end());
        Tools::printSuccess();
    }
}

BankSystem::~BankSystem(){
    clearAccounts();
}