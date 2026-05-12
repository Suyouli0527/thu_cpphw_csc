#include "account.h"
#include <iostream>
#include <string>

Account::Account(int id,char type,const std::string &name,double balance,const Date &openDate)
:id(id),type(type),name(name),balance(balance),openDate(openDate),interest(0.0),lastInterestDate(openDate){};

bool Account::modifyName(const std::string &newName){
        name=newName;
        return true;
}


void Account::settleMonthlyInterest() {
    balance += interest;
    interest = 0.0;
}

void Account::updateInterest(const Date& targetDate) {
    Date current = lastInterestDate;
    
    while (current - targetDate < 0) {
        double daily = InterestCalculator::calcDailyInterest(type, balance, current);
        interest += daily;
        
        current.addDays(1);
        
        if (current.getDay() == 1) {
            settleMonthlyInterest();
        }
    }
    
    lastInterestDate = targetDate;
}


SavingAccount::SavingAccount(int id,char type,const std::string &name,double balance,const Date &openDate):
    Account(id,'S',name,balance,openDate){};


bool SavingAccount::deposit(const Date &date,double amount){
    if(amount<0) {
        return false;
    }
        balance+=amount;
        return true;
}

bool SavingAccount::withdraw(const Date &date,double amount){
      if(amount>balance||amount<0) {
        return false;
      }
      balance-=amount;
      return true;
}


CreditAccount::CreditAccount(int id,char type,const std::string &name,double creditAmount,const Date &openDate):
    Account(id,'C',name,0,openDate),credit(creditAmount){};

bool CreditAccount::deposit(const Date &date,double amount){
    if(amount <0) {
        return false;
    }
    balance += amount;
    return true;
}

bool CreditAccount::withdraw(const Date &date,double amount){
    if(amount <0||amount > balance + credit) {
        return false;
    }
    balance-=amount;
    return true;
}


bool CreditAccount::modifyCredit(double newCredit){
    credit=newCredit;
    return true;
}

void CreditAccount::settleMonthlyInterest(){
    balance+=interest;
    interest=0;
}