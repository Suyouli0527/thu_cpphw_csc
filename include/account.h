#pragma once
#include "date.h"
#include "interestCalculator.h"
#include <string>

class Account{
    protected:
        int id;
        std::string name;
        char type;
        double balance;
        Date openDate;
        Date lastInterestDate;
        double interest;
    public:
        Account(int id,char type,const std::string &name,double balance,const Date &openDate);
        void updateInterest(const Date &targetDate);
        void settleMonthlyInterest();
        
        virtual bool deposit(const Date &date,double amount)=0;
        virtual bool withdraw(const Date &date,double amount)=0;
        int getId() const{return id;};
        char getType()const{return type;} ;
        std::string getName() const {return name;};
        double getBalance() const{return balance;};
        Date getOpenDate() const{return openDate;};
        double getInterest() const {return interest;};

        bool modifyName(const std::string& newName);  
};

class SavingAccount:public Account{
    public:
        SavingAccount(int id,char type,const std::string &name,double balance,const Date &openDate);
        bool deposit(const Date &date,double amount);
        bool withdraw(const Date &date,double amount); 
        void settle(const Date &date);
        void display();
};

class CreditAccount:public Account{
    private:
        double credit;
    public:
        CreditAccount(int id,char type,const std::string &name,double creditAmount,const Date &openDate);
        double getCredit()const{return credit;};
        bool deposit(const Date &date,double amount);
        bool withdraw(const Date &date,double amount);
        bool modifyCredit(double newCredit);
        void settleMonthlyInterest();
        
};