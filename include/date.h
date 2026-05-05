#pragma once
#include <string>
class Date{
    public:
        Date()=default;
        Date(int year,int month,int day);
        int getYear()const {return year;};
        std::string getMonth() const {return month_name;};
        int getDay()const {return day;};
        std::string getweekDay() const {return weekday;};
        int getMaxDay()const;
        bool isleapyear()const{
            return year%4==0&&year%100!=0||year%400==0;
        }
        void showDate()const;
        void addDays(int n);
        void setDays(int y,int m,int d);
        int distance(const Date &date) const{
            return totalDays-date.totalDays;
        }
        void cast();
    private:
        int year=1970;
        int month=1;
        int day=1;
        int totalDays=0;
        std::string month_name=monthName[1];
        std::string weekday=weekName[4];
        const static std::string weekName[7];
        const static std::string monthName[13];
};