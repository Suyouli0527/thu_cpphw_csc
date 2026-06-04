#pragma once
#include "date.h"

class InterestCalculator {
public:
    static const double savingRate;
    static const double creditRate;
    static const double debtRate;
    static double calcDailyInterest(char type, double balance, const Date &date);
};
