#include "account.h"

Account::Account(int id, char type, const std::string &name, double balance, const Date &openDate)
    : id(id), name(name), type(type), balance(balance), openDate(openDate),
      lastInterestDate(openDate), interest(0.0) {}

bool Account::modifyName(const std::string &newName) {
    name = newName;
    return true;
}

void Account::settleMonthlyInterest() {
    balance += interest;
    interest = 0.0;
}

void Account::updateInterest(const Date &targetDate) {
    Date current = lastInterestDate;
    while (current - targetDate < 0) {
        double daily = InterestCalculator::calcDailyInterest(type, balance, current);
        interest += daily;
        current.addDays(1);
        if (current.getDay() == 1) {
            settleMonthlyInterest();
        }
    }
    lastInterestDate = targetDate;
}

SavingAccount::SavingAccount(int id, char, const std::string &name, double balance, const Date &openDate)
    : Account(id, 'S', name, balance, openDate) {}

bool SavingAccount::deposit(const Date &, double amount) {
    if (amount < 0) return false;
    balance += amount;
    return true;
}

bool SavingAccount::withdraw(const Date &, double amount) {
    if (amount < 0 || amount > balance) return false;
    balance -= amount;
    return true;
}

bool SavingAccount::fixedDeposit(const Date &date, double amount, int months) {
    if (amount < 0 || amount > balance) return false;
    if (InterestCalculator::getFixedRate(months) == 0) return false;

    Date maturity = date;
    int days = 0;
    for (int i = 0; i < 6; i++) {
        if (InterestCalculator::fixedMonths[i] == months) {
            days = InterestCalculator::fixedDays[i];
            break;
        }
    }
    if (days == 0) return false;
    maturity.addDays(days);

    balance -= amount;
    fixedDeposits.push_back({amount, months, date, maturity, false});
    return true;
}

bool SavingAccount::fixedWithdraw(const Date &date, double amount) {
    if (amount < 0) return false;

    for (auto it = fixedDeposits.begin(); it != fixedDeposits.end(); ++it) {
        if (date - it->maturityDate < 0 && !it->partiallyWithdrawn && amount <= it->principal) {
            double interest = InterestCalculator::calcEarlyWithdrawInterest(amount, it->depositDate, date);
            balance += amount + interest;
            it->principal -= amount;
            it->partiallyWithdrawn = true;
            return true;
        }
    }
    return false;
}

void SavingAccount::updateFixedDeposits(const Date &date) {
    for (auto it = fixedDeposits.begin(); it != fixedDeposits.end(); ) {
        if (date - it->maturityDate >= 0) {
            double interest = InterestCalculator::calcFixedInterest(it->principal, it->months);
            balance += it->principal + interest;
            it = fixedDeposits.erase(it);
        } else {
            ++it;
        }
    }
}

CreditAccount::CreditAccount(int id, char, const std::string &name, double creditAmount, const Date &openDate)
    : Account(id, 'C', name, 0, openDate), credit(creditAmount) {}

bool CreditAccount::deposit(const Date &, double amount) {
    if (amount < 0) return false;
    balance += amount;
    return true;
}

bool CreditAccount::withdraw(const Date &, double amount) {
    if (amount < 0 || amount > balance + credit) return false;
    balance -= amount;
    return true;
}

bool CreditAccount::modifyCredit(double newCredit) {
    credit = newCredit;
    return true;
}
