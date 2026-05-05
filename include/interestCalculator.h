#pragma once
#include "date.h"
#include<map>

class InterestCalculator{
    private:
        static const double savingRate=0.0115;
        static const double creditRate=0.0025;
        static const double debtRate=0.0005;
    public:
        static double calcSavingDailyInterest(double balance,const Date &date);
        static double calcCreditDailyInterest(double balance,const Date &date);
        static double calcTotalInterest(
            char type,double balance,const Date &startDate,const Date &endDate,double &accuInterest
        );
        static bool ifSettleMonthly(const Date &currentDate,const Date &nextDate);
};


