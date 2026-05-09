#include "date.h"
#include <string>
#include <iostream>
using namespace std;
namespace {
    const int days_before_month[]={0,31,59,90,120,151,181,212,243,273,304,334,365};
}
const std::string Date::weekName[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};

const std::string Date::monthName[13] = {
    "","January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

bool Date::isleapYear(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
};

int Date::daysInMonth(int y, int m) {
    if (m == 2 && isleapYear(y)) return 29;
    return days_before_month[m] - days_before_month[m-1];
};

bool Date::isLegalDate(int y,int m,int d){
    if(m<1||m>12||d<1||d>daysInMonth(y,m)) return false;
    return true;
}

int Date::daysInYear()const {
    return isleapYear(year)?366:365;
}
    // 构造函数
Date::Date(int y, int m, int d) {
    if(!isLegalDate(y,m,d)) throw std::invalid_argument("Invalid date");
    year = y;
    month = m;
    day = d;
    int years = y - 1;
    totalDays = years * 365 + years / 4 - years / 100 + years / 400
                + days_before_month[m-1] + d;
    if ((y % 4 == 0 && y % 100 != 0 || y % 400 == 0) && m > 2)
        totalDays++;
    cast();
};

bool Date::setDays(int y,int m,int d){
    if(!isLegalDate(y,m,d)) {Tools::printFailure(); return false;}
    int years=y-1;
    int newTotal=years*365+years/4-years/100+years/400+days_before_month[m-1]+d;
    if(isleapYear(y)&&m>2) newTotal++;
    if(newTotal<=totalDays) {Tools::printFailure(); return false;}
        year=y;
        month=m;
        day=d;  
        cast();
        totalDays=newTotal;
        Tools::printSuccess();
        return true;
};
int Date::getMaxDay()const{
    if(isleapYear(year)&&month==2)
        return 29;
    else
        return days_before_month[month]-days_before_month[month-1];
};


bool Date::addDays(int n){
if(n>0){
        day += n;
        totalDays+=n;
    while (day > getMaxDay()) {
    day -= getMaxDay();
    month++;
    if (month > 12) {
        month = 1;
        year++;
    }
}
     cast();
    return true;}
     
else{
    return false;}
}

void Date::cast(){
    month_name=monthName[month];
    weekday=weekName[totalDays%7];
}

bool Date::showDate()const {
    cout<<getweekDay()<<", "<<getMonth()<<" "<<getDay()<<", "<<getYear()<<endl;
    return true;
};

