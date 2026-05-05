#include "account.h"
#include <iostream>
#include <cmath>
using namespace std;
double Account::total=0;
Account::Account(const Date &date,const std::string &id):id(id),balance(0){};

void Account::record(const Date &date,double amount,const std::string &desc){
    amount=floor(amount*100+0.5)/100;
    balance+=amount;
    total+=amount;
    date.showDate();
    cout<<" "<<desc<<" "<<amount<<" Balance: "<<balance<<endl;
}

