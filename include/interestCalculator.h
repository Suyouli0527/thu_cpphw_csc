#pragma once
#include "date.h"

class InterestCalculator{
    private:
        static const double savingRate;
        static const double creditRate;
        static const double debtRate;
    public:
        static  double calcDailyInterest(char type,double balance,const Date &date);
};


