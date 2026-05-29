#include "account.h"

Account::Account(int id, char type, const std::string &name, double balance, const Date &openDate, int pwd)
    : id(id), name(name), type(type), balance(balance), openDate(openDate),
      lastInterestDate(openDate), interest(0.0), accountPassword(pwd) {}

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

SavingAccount::SavingAccount(int id, char, const std::string &name, double balance, const Date &openDate, int pwd)
    : Account(id, 'S', name, balance, openDate, pwd) {}

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

CreditAccount::CreditAccount(int id, char, const std::string &name, double creditAmount, int repDay, const Date &openDate, int pwd)
    : Account(id, 'C', name, 0, openDate, pwd), credit(creditAmount), repaymentDay(repDay), lastInterestUpdate(openDate) {}

double CreditAccount::getCashAdvanceDebtInternal() const {
    double cashDebt = 0;
    for (const auto &txn : transactions) {
        if (txn.txnType == 'C') cashDebt += txn.amount;
        else if (txn.txnType == 'R') {
            double remain = -txn.amount;
            if (remain > 0) {
                if (cashDebt >= remain) {
                    cashDebt -= remain;
                } else {
                    remain -= cashDebt;
                    cashDebt = 0;
                }
            }
        }
    }
    return cashDebt;
}

double CreditAccount::getConsumeDebtInternal() const {
    double consumeDebt = 0;
    double cashDebt = 0;
    for (const auto &txn : transactions) {
        if (txn.txnType == 'P') consumeDebt += txn.amount;
        else if (txn.txnType == 'C') cashDebt += txn.amount;
        else if (txn.txnType == 'R') {
            double remain = -txn.amount;
            if (remain > 0) {
                if (cashDebt >= remain) {
                    cashDebt -= remain;
                } else {
                    remain -= cashDebt;
                    cashDebt = 0;
                    if (consumeDebt >= remain) {
                        consumeDebt -= remain;
                    } else {
                        consumeDebt = 0;
                    }
                }
            }
        }
    }
    return consumeDebt;
}

Date CreditAccount::getRepaymentDate(const Date &current) const {
    int y = current.getYear();
    int m = current.getMonth();
    Date rep(y, m, repaymentDay);
    if (rep - current < 0) {
        m++;
        if (m > 12) { m = 1; y++; }
        rep = Date(y, m, repaymentDay);
    }
    return rep;
}

bool CreditAccount::isWithinGracePeriod(const Date &txnDate, const Date &checkDate) const {
    int txnMonth = txnDate.getMonth();
    int txnYear = txnDate.getYear();
    int nextMonth = txnMonth + 1;
    int nextYear = txnYear;
    if (nextMonth > 12) { nextMonth = 1; nextYear++; }
    Date graceDeadline(nextYear, nextMonth, repaymentDay);
    return checkDate - graceDeadline < 0;
}

double CreditAccount::getTotalDebt() const {
    return getCashAdvanceDebt() + getConsumeDebt();
}

double CreditAccount::getCashAdvanceDebt() const {
    return getCashAdvanceDebtInternal();
}

double CreditAccount::getConsumeDebt() const {
    return getConsumeDebtInternal();
}

bool CreditAccount::deposit(const Date &date, double amount) {
    if (amount < 0) return false;
    double cashDebt = getCashAdvanceDebtInternal();
    double consumeDebt = getConsumeDebtInternal();
    double remaining = amount;
    if (cashDebt > 0 && remaining > 0) {
        double repay = remaining < cashDebt ? remaining : cashDebt;
        remaining -= repay;
    }
    if (consumeDebt > 0 && remaining > 0) {
        double repay = remaining < consumeDebt ? remaining : consumeDebt;
        remaining -= repay;
    }
    balance += amount;
    transactions.push_back({-amount, 'R', date});
    return true;
}

bool CreditAccount::withdraw(const Date &date, double amount) {
    return cashAdvance(date, amount);
}

bool CreditAccount::consume(const Date &date, double amount) {
    if (amount < 0 || amount > balance + credit) return false;
    balance -= amount;
    transactions.push_back({amount, 'P', date});
    return true;
}

bool CreditAccount::cashAdvance(const Date &date, double amount) {
    if (amount < 0 || amount > balance + credit) return false;
    balance -= amount;
    transactions.push_back({amount, 'C', date});
    return true;
}

void CreditAccount::updateCreditInterest(const Date &targetDate) {
    double cashDebt = getCashAdvanceDebtInternal();
    double consumeDebt = getConsumeDebtInternal();
    Date current = lastInterestUpdate;

    while (current - targetDate < 0) {
        Date next = current;
        next.addDays(1);
        double dailyInterest = 0;

        // 取现利息：从交易次日到还款日或当前
        for (const auto &txn : transactions) {
            if (txn.txnType != 'C') continue;
            Date interestStart = txn.date;
            interestStart.addDays(1);
            if (next - interestStart > 0) {
                dailyInterest += txn.amount * InterestCalculator::debtRate;
            }
        }

        // 消费利息：已过免息期的部分
        for (const auto &txn : transactions) {
            if (txn.txnType != 'P') continue;
            if (!isWithinGracePeriod(txn.date, next)) {
                dailyInterest += txn.amount * InterestCalculator::debtRate;
            }
        }

        balance -= dailyInterest;
        current = next;
    }
    lastInterestUpdate = targetDate;
}

bool CreditAccount::modifyCredit(double newCredit) {
    credit = newCredit;
    return true;
}
