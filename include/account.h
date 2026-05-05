#pragma once
#include "date.h"
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
        virtual void deposit(const Date &date,int amount)=0;
        virtual void withdraw(const Date &date,int amount)=0;
        virtual void transfer(Account &target,double amount)=0;
        virtual void calcDailyInterest(const Date &date)=0;
        virtual void settleMonthlyInterest()=0;
        
        virtual void getdailyRate()=0;
        int getId() const{return id;};
        char getType()const{return type;} ;
        std::string getName() const {return name;};
        double getBalance() const{return balance;};
        Date getOpenDate() const{return openDate;};
        double getInterest() const {return interest;};

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
        void calcDailyInterest(const Date &date);
        void settleMonthlyInterest();
        void transfer(Account &target,double amount);
        void getdailyRate();
};

class CreditAccount:public Account{
    private:
        static const double creditRate=0.0225;
        static const double debtRate=0.0005;
        double credit;
    public:
         CreditAccount(const int &id,char type,const std::string &name,int credit,const Date &openDate);
        void deposit(const Date &date,int amount);
        void withdraw(const Date &date,int amount);
        void settle(const Date &date);
        void calcDailyInterest(const Date &date);
        void settleMonthlyInterest();
        void transfer(Account &target,double amount);
        void getdailyRate();
};