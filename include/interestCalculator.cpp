#include "interestCalculator.h"
const double InterestCalculator::savingRate = 0.0115;
const double InterestCalculator::creditRate = 0.0225;
const double InterestCalculator::debtRate = 0.0005;
double  InterestCalculator::calcDailyInterest(char type,double balance,const Date &date){
    double dailyRate;
    switch(type){
        case 'S':
            dailyRate = savingRate/date.daysInYear();
            break;
        case 'C':
            if(balance>=0){
                dailyRate = creditRate/date.daysInYear();
            }
            else{
                dailyRate = debtRate;
            }
            break;
        default:
            return 0;
    }
    return balance*dailyRate;
}



        

