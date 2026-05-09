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
        std::vector<User> users;
        std::vector<Account*> accounts;
        std::vector<std::string> logRecords;
        
        Account* findAccount(int id) const;
        User* findUser(const std::string &name) const;
        void updateAllAccountsInterest(const Date &newDate);
        void removeAccount(int id);
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
        void queryAllAccounts()const;

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
        
        void showLog() const;
        void rollback(int n);
        
        bool isLoggedIn() const { return currentUser != nullptr; }
        bool isAdmin() const { return isLoggedIn() && currentUser->isAdmin(); }
        bool islegalName(const std::string &name) const;
};