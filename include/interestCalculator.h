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

    // 基金产品表
    static const int fundCount;
    static const double fundBaseNav[];
    static const double fundVolatility[];
    static const char* fundNames[];

    // 理财产品表
    static const int wpCount;
    static const double wpRates[];
    static const int wpDays[];
    static const char* wpNames[];

    // 基金净值计算（确定性，保证回滚一致性）
    static double getNav(int fundIndex, const Date &date);
    static double calcWealthInterest(double principal, int productIndex);
    static const char* getFundName(int fundIndex);
    static const char* getWPName(int productIndex);
    static int getWPDays(int productIndex);
};
