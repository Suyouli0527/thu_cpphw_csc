#pragma once
#include <string>

class Date {
public:
    Date();
    Date(int year, int month, int day);

    int getYear() const { return year; }
    int getMonth() const { return month; }
    int getDay() const { return day; }
    int getTotalDays() const { return totalDays; }
    std::string getMonthName() const { return monthName[month]; }
    std::string getWeekDay() const { return weekName[totalDays % 7]; }
    int getMaxDay() const;
    int daysInYear() const;

    static bool isLeapYear(int y);
    static int daysInMonth(int y, int m);
    static bool isLegalDate(int y, int m, int d);

    bool showDate() const;
    bool addDays(int n);
    bool setDate(int y, int m, int d);

    int operator-(const Date &other) const {
        return totalDays - other.totalDays;
    }

private:
    int year;
    int month;
    int day;
    int totalDays;

    static const std::string weekName[7];
    static const std::string monthName[13];
    static const int daysBeforeMonth[13];
};
