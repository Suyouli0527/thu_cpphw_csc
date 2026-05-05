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
        Account(const int &id,char type,const std::string &name,double balance,const Date &openDate);
        virtual void deposit(const Date &date,double amount)=0;
        virtual void withdraw(const Date &date,double amount)=0;
        virtual void transfer(const Date &date,Account &target,double amount)=0;
        virtual void calcDailyInterest(const Date &date)=0;
        virtual void settleMonthlyInterest()=0;
        
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
        SavingAccount(const int &id,char type,const std::string &name,int balance,const Date &openDate);
        void deposit(const Date &date,double amount);
        void withdraw(const Date &date,double amount); 
        void transfer(const Date &date,Account &target,double amount);
        void settle(const Date &date);
        void calcDailyInterest(const Date &date);
        void settleMonthlyInterest();
        void display();
};

class CreditAccount:public Account{
    private:
        double credit;
    public:
        CreditAccount(const int &id,char type,const std::string &name,double balance,const Date &openDate,double credit);
        double getCredit()const{return credit;};
        void deposit(const Date &date,double amount);
        void withdraw(const Date &date,double amount);
        void transfer(const Date &date,Account &target,double amount);
        bool modifyCredit(double newCredit);
        void calcDailyInterest(const Date &date);
        void settleMonthlyInterest();
        
};