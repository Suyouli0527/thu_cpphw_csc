#include "account.h"

Account::Account(int id, char type, const std::string &name, double balance, const Date &openDate, const std::string &pwd, bool isShared)
    : id(id), name(name), type(type), balance(balance), openDate(openDate),
      lastInterestDate(openDate), interest(0.0), accountPassword(pwd), shared(isShared), frozen(false) {}

std::vector<std::string> Account::getOwnerNames() const {
    std::vector<std::string> names;
    for (const auto &o : owners) {
        names.push_back(o.name);
    }
    return names;
}

void Account::addOwner(const std::string &userName, OwnerLevel level, double withdrawLimit) {
    if (owners.size() >= 5) return;
    for (const auto &o : owners) {
        if (o.name == userName) return;
    }
    owners.push_back({userName, level, level == OwnerLevel::RESTRICTED ? withdrawLimit : 0.0});
}

void Account::removeOwner(const std::string &userName) {
    if (owners.size() <= 1) return;
    for (auto it = owners.begin(); it != owners.end(); ++it) {
        if (it->name == userName) {
            owners.erase(it);
            return;
        }
    }
}

bool Account::modifyOwnerLimit(const std::string &userName, double newLimit) {
    for (auto &o : owners) {
        if (o.name == userName && o.level == OwnerLevel::RESTRICTED) {
            o.withdrawLimit = newLimit;
            return true;
        }
    }
    return false;
}

OwnerLevel Account::getOwnerLevel(const std::string &userName) const {
    for (const auto &o : owners) {
        if (o.name == userName) return o.level;
    }
    return OwnerLevel::RESTRICTED;
}

double Account::getWithdrawLimit(const std::string &userName) const {
    for (const auto &o : owners) {
        if (o.name == userName) return o.withdrawLimit;
    }
    return 0;
}

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

SavingAccount::SavingAccount(int id, char, const std::string &name, double balance, const Date &openDate, const std::string &pwd, bool isShared)
    : Account(id, 'S', name, balance, openDate, pwd, isShared) {}

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

bool SavingAccount::fixedDeposit(const Date &date, double amount, int months, bool autoRenew) {
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
    fixedDeposits.push_back({amount, months, date, maturity, false, autoRenew});
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
            if (it->autoRenew) {
                double interest = InterestCalculator::calcFixedInterest(it->principal, it->months);
                double newPrincipal = it->principal + interest;
                int days = 0;
                for (int i = 0; i < 6; i++) {
                    if (InterestCalculator::fixedMonths[i] == it->months) {
                        days = InterestCalculator::fixedDays[i];
                        break;
                    }
                }
                Date newMaturity = date;
                newMaturity.addDays(days);
                it->principal = newPrincipal;
                it->depositDate = date;
                it->maturityDate = newMaturity;
                ++it;
            } else {
                double interest = InterestCalculator::calcFixedInterest(it->principal, it->months);
                balance += it->principal + interest;
                it = fixedDeposits.erase(it);
            }
        } else {
            ++it;
        }
    }
}

bool SavingAccount::setAutoRenew(int index, bool autoRenew) {
    if (index < 0 || index >= (int)fixedDeposits.size()) return false;
    fixedDeposits[index].autoRenew = autoRenew;
    return true;
}

CreditAccount::CreditAccount(int id, char, const std::string &name, double creditAmount, int repDay, const Date &openDate, const std::string &pwd, bool isShared)
    : Account(id, 'C', name, 0, openDate, pwd, isShared), credit(creditAmount),
      repaymentDay(repDay), cash_debt(0), consume_debt(0), consume_debt_overdue(0),
      lastInterestUpdate(openDate) {}

bool CreditAccount::isWithinGracePeriod(const Date &consumeDate, const Date &checkDate) const {

    int y = consumeDate.getYear();
    int m = consumeDate.getMonth();
    m++;
    if (m > 12) { m = 1; y++; }

    int maxDay = Date::daysInMonth(y, m);
    int day = std::min(repaymentDay, maxDay);
    Date graceDeadline(y, m, day);
    return checkDate - graceDeadline < 0;
}

bool CreditAccount::deposit(const Date &, double amount) {
    if (amount < 0) return false;
    balance += amount;
    double remain = balance;
    if (remain <= 0) return true;
    if (cash_debt > 0 && remain > 0) {
        if (cash_debt >= remain) { cash_debt -= remain; remain = 0; }
        else { remain -= cash_debt; cash_debt = 0; }
    }
    if (remain > 0 && consume_debt_overdue > 0) {
        if (consume_debt_overdue >= remain) { consume_debt_overdue -= remain; remain = 0; }
        else { remain -= consume_debt_overdue; consume_debt_overdue = 0; }
    }
    if (remain > 0 && consume_debt > 0) {
        if (consume_debt >= remain) { consume_debt -= remain; remain = 0; }
        else { consume_debt = 0; }
    }
    return true;
}

bool CreditAccount::withdraw(const Date &, double amount) {
    if (amount < 0 || amount > balance + credit) return false;
    balance -= amount;
    if (balance < 0) {
        double overdraft = -balance;
        cash_debt += overdraft;
    }
    return true;
}

bool CreditAccount::consume(const Date &, double amount) {
    if (amount < 0 || amount > balance + credit) return false;
    balance -= amount;
    if (balance < 0) {
        double overdraft = -balance;
        consume_debt += overdraft;
    }
    return true;
}

void CreditAccount::updateCreditInterest(const Date &targetDate) {
    Date current = lastInterestUpdate;

    while (current - targetDate < 0) {
        Date next = current;
        next.addDays(1);

        int maxDay = Date::daysInMonth(next.getYear(), next.getMonth());
        int day = std::min(repaymentDay, maxDay);
        if (next.getDay() == day) {
            consume_debt_overdue += consume_debt;
            consume_debt = 0;
        }

        double dailyInterest = 0;
        dailyInterest += cash_debt * InterestCalculator::debtRate;
        dailyInterest += consume_debt_overdue * InterestCalculator::debtRate;

        balance -= dailyInterest;
        current = next;
    }
    lastInterestUpdate = targetDate;
}

bool CreditAccount::modifyCredit(double newCredit) {
    credit = newCredit;
    return true;
}

bool CreditAccount::modifyRepaymentDay(int newDay) {
    if (newDay < 1 || newDay > 28) return false;
    repaymentDay = newDay;
    return true;
}