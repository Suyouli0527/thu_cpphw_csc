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
    Date d = lastInterestDate;
    d.addDays(1);                     
    while (d - targetDate <= 0) {      
        double daily = InterestCalculator::calcDailyInterest(type, balance, d);
        interest += daily;

        Date nextDay = d;
        nextDay.addDays(1);
        if (nextDay.getDay() == 1) {
            settleMonthlyInterest();  
        }

        d = nextDay;
    }
    lastInterestDate = targetDate;
}


SavingAccount::SavingAccount(int id,char type,const std::string &name,double balance,const Date &openDate):
    Account(id,'s',name,balance,openDate){};


bool SavingAccount::deposit(const Date &date,double amount){
    if(amount<0) {
        Tools::printFailure();
        return false;
    }
        balance+=amount;
        Tools::printSuccess();
        return true;
}

bool SavingAccount::withdraw(const Date &date,double amount){
      if(amount>balance||amount<0) {
        Tools::printFailure();
        return false;
      }
      balance-=amount;
      Tools::printSuccess();
      return true;
}

bool SavingAccount::transfer(const Date &date,Account &target,double amount){
    if(amount <0||amount > balance) {
        Tools::printFailure();
        return false;
    }
    balance-=amount;
    target.deposit(date,amount);
    Tools::printSuccess();
    return true;
}

CreditAccount::CreditAccount(int id,char type,const std::string &name,double creditAmount,const Date &openDate):
    Account(id,'c',name,0,openDate),credit(creditAmount){};

bool CreditAccount::deposit(const Date &date,double amount){
    if(amount <=0) {
        Tools::printFailure();
        return false;
    }
    balance += amount;
    Tools::printSuccess();
    return true;
}

bool CreditAccount::withdraw(const Date &date,double amount){
    if(amount <0||amount > balance + credit) {
        Tools::printFailure();
        return false;
    }
    balance-=amount;
    Tools::printSuccess();
    return true;
}

bool CreditAccount::transfer(const Date &date,Account &target,double amount){
    if(amount <0||amount > balance + credit) {
        Tools::printFailure();
        return false;
    }
    balance-=amount;
    target.deposit(date,amount);
    Tools::printSuccess();
    return true;
}

bool CreditAccount::modifyCredit(double newCredit){
    credit=newCredit;
    Tools::printSuccess();
    return true;
}

void CreditAccount::settleMonthlyInterest(){
    balance+=interest;
    interest=0;
}