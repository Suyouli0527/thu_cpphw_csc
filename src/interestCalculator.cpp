#include "interestCalculator.h"

double InterestCalculator::calcSavingDailyInterest(double balance,const Date &date){
    double dailyRate =savingRate/date.daysInYear();
    return balance*dailyRate;
}


double InterestCalculator::calcCreditDailyInterest(double balance,const Date &date){
    if(balance>=0){
        double dailyRate=creditRate/date.daysInYear();
        return balance*dailyRate;
    }
    else{
        return balance*debtRate;
    }
}

double InterestCalculator::calcTotalInterest(
            char type,
            double balance,
            const Date &startDate,
            const Date &endDate,
            double &accuInterest
        ){
            double totalInterest =0;
            Date currentDate = startDate;
            while(currentDate-endDate<0){
                double dailyInterest;
                if(type=='s'){
                    dailyInterest=calcSavingDailyInterest(balance,currentDate);
                }
                else{
                    dailyInterest=calcCreditDailyInterest(balance,currentDate);
                }
                accuInterest+=dailyInterest;

                Date nextDay= currentDate;
                nextDay.addDays(1);
                    if(ifSettleMonthly(currentDate,nextDay)){
                        balance+=accuInterest;
                        totalInterest+=accuInterest;
                        accuInterest=0;
                    }
                currentDate.addDays(1);
            }
        }
bool InterestCalculator::ifSettleMonthly(const Date &currentDate,const Date &nextDate){
    return nextDate.getMonth()!=currentDate.getMonth();
}
        

