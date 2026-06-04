#include "dateManager.h"
#include "accountManager.h"

DateManager::DateManager(AccountManager* am)
    : currentDate(1970, 1, 1), accountMgr(am) {}

void DateManager::updateAllAccountsInterest(const Date &newDate) {
    if (!accountMgr) return;
    for (auto acc : accountMgr->getAccounts()) {
        acc->updateInterest(newDate);
    }
}

void DateManager::showDate() const {
    currentDate.showDate();
}

bool DateManager::addDays(int days) {
    if (!currentDate.addDays(days)) return false;
    updateAllAccountsInterest(currentDate);
    return true;
}

bool DateManager::setDate(int year, int month, int day) {
    if (!currentDate.setDate(year, month, day)) return false;
    updateAllAccountsInterest(currentDate);
    return true;
}

void DateManager::reset() {
    currentDate = Date(1970, 1, 1);
}
