#include "interestCalculator.h"
#include <cmath>

const double InterestCalculator::savingRate = 0.0115;
const double InterestCalculator::creditRate = 0.0225;
const double InterestCalculator::debtRate = 0.0005;

const int InterestCalculator::fixedMonths[] = {3, 6, 12, 24, 36, 60};
const double InterestCalculator::fixedRates[] = {0.0135, 0.0155, 0.0175, 0.0225, 0.0275, 0.0300};
const int InterestCalculator::fixedDays[] = {90, 180, 365, 730, 1095, 1825};

// 基金产品表
const int InterestCalculator::fundCount = 4;
const double InterestCalculator::fundBaseNav[] = {1.0, 1.5, 2.0, 1.0};
const double InterestCalculator::fundVolatility[] = {0.01, 0.03, 0.08, 0.003};
const char* InterestCalculator::fundNames[] = {"稳健债券", "成长混合", "科技股票", "货币基金"};

// 理财产品表
const int InterestCalculator::wpCount = 4;
const double InterestCalculator::wpRates[] = {0.025, 0.030, 0.040, 0.050};
const int InterestCalculator::wpDays[] = {30, 90, 180, 365};
const char* InterestCalculator::wpNames[] = {"短期理财A", "短期理财B", "中期理财C", "长期理财D"};

double InterestCalculator::calcDailyInterest(char type, double balance, const Date &date) {
    double dailyRate;
    switch (type) {
        case 'S':
            dailyRate = savingRate / date.daysInYear();
            break;
        case 'C':
            if (balance >= 0) {
                dailyRate = creditRate / date.daysInYear();
            } else {
                dailyRate = debtRate;
            }
            break;
        default:
            return 0;
    }
    return balance * dailyRate;
}

double InterestCalculator::getFixedRate(int months) {
    for (int i = 0; i < 6; i++) {
        if (fixedMonths[i] == months) return fixedRates[i];
    }
    return 0;
}

double InterestCalculator::calcFixedInterest(double principal, int months) {
    double rate = getFixedRate(months);
    return principal * rate * months / 12.0;
}

double InterestCalculator::calcEarlyWithdrawInterest(double principal, const Date &depositDate, const Date &withdrawDate) {
    int days = withdrawDate - depositDate;
    if (days <= 0) return 0;
    return principal * savingRate * days / 365.0;
}

double InterestCalculator::getNav(int fundIndex, const Date &date) {
    double base = fundBaseNav[fundIndex];
    double vol = fundVolatility[fundIndex];
    int td = date.getTotalDays();
    return base * (1.0 + vol * std::sin(td * 0.1 + fundIndex * 1.5));
}

double InterestCalculator::calcWealthInterest(double principal, int productIndex) {
    double rate = wpRates[productIndex];
    int days = wpDays[productIndex];
    return principal * rate * days / 365.0;
}

const char* InterestCalculator::getFundName(int fundIndex) {
    return fundNames[fundIndex];
}

const char* InterestCalculator::getWPName(int productIndex) {
    return wpNames[productIndex];
}

int InterestCalculator::getWPDays(int productIndex) {
    return wpDays[productIndex];
}
