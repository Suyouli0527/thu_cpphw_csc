#include "date.h"
#include <iostream>
#include <stdexcept>

const std::string Date::weekName[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};

const std::string Date::monthName[13] = {
    "", "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

const int Date::daysBeforeMonth[13] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};

bool Date::isLeapYear(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

int Date::daysInMonth(int y, int m) {
    if (m == 2 && isLeapYear(y)) return 29;
    return daysBeforeMonth[m] - daysBeforeMonth[m - 1];
}

bool Date::isLegalDate(int y, int m, int d) {
    if (m < 1 || m > 12 || d < 1) return false;
    return d <= daysInMonth(y, m);
}

Date::Date() : year(1970), month(1), day(1) {
    totalDays = 719163;
}

Date::Date(int y, int m, int d) {
    if (!isLegalDate(y, m, d)) {
        throw std::invalid_argument("Invalid date");
    }
    year = y;
    month = m;
    day = d;
    int years = y - 1;
    totalDays = years * 365 + years / 4 - years / 100 + years / 400
                + daysBeforeMonth[m - 1] + d;
    if (isLeapYear(y) && m > 2) {
        totalDays++;
    }
}

int Date::getMaxDay() const {
    return daysInMonth(year, month);
}

int Date::daysInYear() const {
    return isLeapYear(year) ? 366 : 365;
}

bool Date::showDate() const {
    std::cout << getWeekDay() << ", " << getMonthName() << " "
              << day << ", " << year << std::endl;
    return true;
}

bool Date::addDays(int n) {
    if (n <= 0) return false;
    day += n;
    totalDays += n;
    while (day > getMaxDay()) {
        day -= getMaxDay();
        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }
    return true;
}

bool Date::setDate(int y, int m, int d) {
    if (!isLegalDate(y, m, d)) return false;
    int years = y - 1;
    int newTotal = years * 365 + years / 4 - years / 100 + years / 400
                   + daysBeforeMonth[m - 1] + d;
    if (isLeapYear(y) && m > 2) newTotal++;
    if (newTotal <= totalDays) return false;
    year = y;
    month = m;
    day = d;
    totalDays = newTotal;
    return true;
}
