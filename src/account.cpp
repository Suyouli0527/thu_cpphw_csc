#include "account.h"
#include "date.h"
#include <cmath>
#include <iostream>
#include <string>

Account::Account(const int &id,char type,const std::string &name,int balance,const Date &openDate):
    id(id),type(type),name(name),openDate(openDate),balance(balance){};



SavingAccount::SavingAccount(const int &id,char type,const std::string &name,int balance,const Date &openDate):
    Account(id,type,name,balance,openDate){};

void SavingAccount::deposit(const Date &date,int amount){
        balance+=amount;
}

void SavingAccount::withdraw(const Date &date,int amount){
        balance-=amount;
}




void SavingAccount::calcDailyInterest(const Date &date){
    double dailyRate=savingRate/date.daysInYear();
    interest+=balance*dailyRate;
}

void SavingAccount::settleMonthlyInterest(){
    balance += interest;
    interest=0;
}

CreditAccount::CreditAccount(const int &id,char type,const std::string &name,int credit,const Date &openDate):
    Account(id,type,name,0,openDate),credit(credit){};

