#include "date.h"
#include <string>
#include <iostream>
const std::string Date::weekName[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};

const std::string Date::monthName[13] = {
    "","January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

using namespace std;
namespace {
    const int days_before_month[]={0,31,59,90,120,151,181,212,243,273,304,334,365};
}
void Date::setDays(int y,int m,int d){
        /*添加拦截函数*/
        year=y;
        month=m;
        day=d;  
        int years=y-1;
        totalDays=years*365+years/4-years/100+years/400+days_before_month[month-1]+day;
        if(isleapyear()&&month>2) totalDays++;
        cast();
};

Date::Date(int y,int m,int d){
    setDays(y,m,d);

};
int Date::getMaxDay()const{
    if(isleapyear()&&month==2)
        return 29;
    else
        return days_before_month[month]-days_before_month[month-1];
};




void Date::addDays(int n){
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
}

void Date::cast(){
    month_name=monthName[month];
    weekday=weekName[totalDays%7];
}

void Date::showDate()const {
    cout<<getweekDay()<<", "<<getMonth()<<" "<<getDay()<<", "<<getYear()<<endl;
};