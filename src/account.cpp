#include "account.h"
#include <cmath>
#include <iostream>
#include <string>

Account::Account(const int &id,char type,const std::string &name,int balance,const Date &openDate):
    id(id),type(type),name(name),opendate(openDate),balance(balance){};

SavingAccount::SavingAccount(const int &id,char type,const std::string &name,int balance,const Date &openDate):
    Account(id,type,name,balance,openDate){};

void SavingAccount::deposit(const Date &date,int amount){
        balance+=amount;
}

void SavingAccount::withdraw(const Date &date,int amount){
        balance-=amount;
}

void  SavingAccount::transfer(Account& target,double amount){
    balance-=amount;
    target.balance+=amount;
}

