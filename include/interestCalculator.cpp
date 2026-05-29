#include "interestCalculator.h"

const double InterestCalculator::savingRate = 0.0115;
const double InterestCalculator::creditRate = 0.0225;
const double InterestCalculator::debtRate = 0.0005;

const int InterestCalculator::fixedMonths[] = {3, 6, 12, 24, 36, 60};
const double InterestCalculator::fixedRates[] = {0.0135, 0.0155, 0.0175, 0.0225, 0.0275, 0.0300};
const int InterestCalculator::fixedDays[] = {90, 180, 365, 730, 1095, 1825};

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
