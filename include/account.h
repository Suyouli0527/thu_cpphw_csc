#pragma once
#include "date.h"
#include <string>

class Account{
    protected:
        int id;
        std::string name;
        std::string owner;
        char type;
        double balance;
        Date lastDate;
        double interest;
        double dailyRate;
    public:
        Account(const int &id,char type,const std::string &name,int balance,const Date &lastDate);
        virtual void deposit(const Date &date,int amount)=0;
        virtual void withdraw(const Date &date,int amount)=0;
        virtual void settle(const Date &date)=0;
        virtual void settleMonthlyInterest()=0;
        virtual void transfer(Account &target,double amount)=0;
        virtual void getdailyRate()=0;
        int getId() const{return id;};
        char getType()const{return type;} ;
        std::string getName() const {return name;};
        double getBalance() const{return balance;};
        Date getOpenDate() const{return lastDate;};
        bool modifyName(const std::string& newName);        
};

class SavingAccount:public Account{
    private:
        static const double savingRate=0.0115;
    public:
        SavingAccount(const int &id,char type,const std::string &name,int balance,const Date &openDate);
        void deposit(const Date &date,int amount);
        void withdraw(const Date &date,int amount);
        void settle(const Date &date);
};