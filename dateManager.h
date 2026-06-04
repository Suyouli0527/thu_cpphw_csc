#pragma once
#include "date.h"

class AccountManager;

class DateManager {
private:
    Date currentDate;
    AccountManager* accountMgr;

    void updateAllAccountsInterest(const Date &newDate);

public:
    DateManager(AccountManager* am);

    void showDate() const;
    bool addDays(int days);
    bool setDate(int year, int month, int day);

    const Date& getCurrentDate() const { return currentDate; }
    void reset();
};
