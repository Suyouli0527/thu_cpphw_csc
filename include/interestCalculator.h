#pragma once
#include "date.h"
class InterestCalculator {
public:
    static const double savingRate;
    static const double creditRate;
    static const double debtRate;
    static const int fixedMonths[];
    static const double fixedRates[];
    static const int fixedDays[];

    static double calcDailyInterest(char type, double balance, const Date &date);
    static double getFixedRate(int months);
    static double calcFixedInterest(double principal, int months);
    static double calcEarlyWithdrawInterest(double principal, const Date &depositDate, const Date &withdrawDate);
};
