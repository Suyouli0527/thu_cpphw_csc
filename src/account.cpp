#include "account.h"
#include <iostream>
#include <string>

Account::Account(const int &id,char type,const std::string &name,double balance,const Date &openDate):id(id),type(type),name(name),balance(balance),openDate(openDate){};

bool Account::modifyName(const std::string &newName){
        name=newName;
        return true;
}

void Account::calcDailyInterest(const Date &targetDate){
    InterestCalculator::calcTotalInterest(
        type,
        balance,
        lastInterestDate,
        targetDate,
        interest
    );
    lastInterestDate = targetDate;}


SavingAccount::SavingAccount(const int &id,char type,const std::string &name,int balance,const Date &openDate):
    Account(id,'s',name,balance,openDate){};


void SavingAccount::deposit(const Date &date,double amount){
        balance+=amount;
}

void SavingAccount::withdraw(const Date &date,double amount){
        balance-=amount;
}

void SavingAccount::transfer(const Date &date,Account &target,double amount){
    balance-=amount;
    target.deposit(date,amount);
}

void SavingAccount::settleMonthlyInterest(){
    balance+=interest;
    interest=0;
}
CreditAccount::CreditAccount(const int &id,char type,const std::string &name,double balance,const Date &openDate,double credit):
    Account(id,'c',name,0,openDate),credit(credit){};

void CreditAccount::deposit (const Date &date,double amount){
    balance+=amount;
}

void CreditAccount::withdraw(const Date &date,double amount){
    balance-=amount;
}

void CreditAccount::transfer(const Date &date,Account &target,double amount){
    balance-=amount;
    target.deposit(date,amount);
}

bool CreditAccount::modifyCredit(double newCredit){
    credit=newCredit;
}

void CreditAccount::calcDailyInterest(const Date &date){
    interest+=InterestCalculator::calcCreditDailyInterest(balance,date);
}
void CreditAccount::settleMonthlyInterest(){
    balance+=interest;
    interest=0;
}