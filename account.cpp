#include "account.h"
#include <iostream>
#include <cmath>
using namespace std;
double Account::total=0;
Account::Account(const Date &date,const std::string &id):id(id),balance(0)
{date.show();
    cout<<" Account "<<id<<" is created."<<endl;
}

void Account::record(const Date &date,double amount,const std::string &desc){
    amount=floor(amount*100+0.5)/100;
    balance+=amount;
    total+=amount;
    date.show();
    cout<<" "<<desc<<" "<<amount<<" Balance: "<<balance<<endl;
}

