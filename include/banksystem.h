#pragma once
#include "date.h"
#include "user.h"
#include "account.h"
#include "tools.h"
#include <vector>
#include <string>

class BankSystem {
    private:
        Date currentDate;
        User  *currentUser;
        Account *currentAccount;
        std::vector<User> users;
        std::vector<Account*> accounts;
        
        Account* findAccount(int id) const;
        User* findUser(const std::string &name) const;
        void updateAllAccountsInterest(const Date &newDate);
        void removeAccount();
    public:
        BankSystem(); 
        ~BankSystem();
        void clearAccounts();
        BankSystem(const BankSystem&) = delete;
        BankSystem& operator=(const BankSystem&) = delete;

        void Command(const std::string &command);


        void openAccount(int id,char type,std::string accountName,double balance);
        void closeAccount(int id);
        void modifyName(int id,const std::string &username);
        void modifyCredit(int id,double newCredit);
        void query(int id) const;
        void queryallAccounts()const;

        void deposit(int id, double amount);
        void withdraw(int id, double amount);
        void transfer(int srcId, int dstId, double amount);
    
        void showDate() const;
        void addDays(int days);
        void setDate(int year, int month, int day);

        void createUser(const std::string &username);
        void deleteUser(const std::string &username);
     
        void queryUser(const std::string &username) const;
        void queryAllUser() const;

        void switchUser(const std::string &username);
        void whoami() const;
        
        bool isLoggedIn() const { return currentUser != nullptr; }
        bool isAdmin() const { return isLoggedIn() && currentUser->isAdmin(); }
};

//新建一个cmd类用于管理命令和函数的映射关系