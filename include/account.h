#pragma once
#include "date.h"
#include "interestCalculator.h"
#include <string>
#include <vector>

enum class OwnerLevel { FULL, RESTRICTED };

struct OwnerInfo {
    std::string name;
    OwnerLevel level;
    double withdrawLimit;
};

class Account {
protected:
    int id;
    std::string name;
    char type;
    double balance;
    Date openDate;
    Date lastInterestDate;
    double interest;
    std::string accountPassword;
    bool shared;
    bool frozen;
    std::vector<OwnerInfo> owners;

public:
    Account(int id, char type, const std::string &name, double balance, const Date &openDate, const std::string &pwd, bool isShared);
    virtual ~Account() = default;

    void updateInterest(const Date &targetDate);
    void settleMonthlyInterest();

    virtual bool deposit(const Date &date, double amount) = 0;
    virtual bool withdraw(const Date &date, double amount) = 0;

    int getId() const { return id; }
    char getType() const { return type; }
    std::string getName() const { return name; }
    double getBalance() const { return balance; }
    Date getOpenDate() const { return openDate; }
    bool isShared() const { return shared; }
    void setShared(bool s) { shared = s; }
    bool isFrozen() const { return frozen; }
    void setFrozen(bool f) { frozen = f; }
    const std::vector<OwnerInfo>& getOwners() const { return owners; }
    std::vector<std::string> getOwnerNames() const;
    void addOwner(const std::string &userName, OwnerLevel level, double withdrawLimit);
    void removeOwner(const std::string &userName);
    bool modifyOwnerLimit(const std::string &userName, double newLimit);
    OwnerLevel getOwnerLevel(const std::string &userName) const;
    double getWithdrawLimit(const std::string &userName) const;
    static bool isValidPassword(const std::string &pwd) {
        if (pwd.length() != 6) return false;
        for (char c : pwd) {
            if (c < '0' || c > '9') return false;
        }
        return true;
    }
    bool verifyAccountPassword(const std::string &pwd) const { return accountPassword == pwd; }
    bool changeAccountPassword(const std::string &oldPwd, const std::string &newPwd) {
        if (accountPassword != oldPwd) return false;
        if (!isValidPassword(newPwd)) return false;
        accountPassword = newPwd;
        return true;
    }

    bool modifyName(const std::string &newName);
};

struct FixedDeposit {
    double principal;
    int months;
    Date depositDate;
    Date maturityDate;
    bool partiallyWithdrawn;
    bool autoRenew;
};

class SavingAccount : public Account {
    std::vector<FixedDeposit> fixedDeposits;

public:
    SavingAccount(int id, char type, const std::string &name, double balance, const Date &openDate, const std::string &pwd, bool isShared);
    bool deposit(const Date &date, double amount) override;
    bool withdraw(const Date &date, double amount) override;

    bool fixedDeposit(const Date &date, double amount, int months, bool autoRenew);
    bool fixedWithdraw(const Date &date, double amount);
    bool setAutoRenew(int index, bool autoRenew);
    void updateFixedDeposits(const Date &date);
    std::vector<FixedDeposit>& getFixedDeposits() { return fixedDeposits; }
    const std::vector<FixedDeposit>& getFixedDeposits() const { return fixedDeposits; }
    int getFixedDepositCount() const { return (int)fixedDeposits.size(); }

};

class CreditAccount : public Account {
private:
    double credit;
    int repaymentDay;
    double cash_debt;
    double consume_debt;
    double consume_debt_overdue;
    Date lastInterestUpdate;

    bool isWithinGracePeriod(const Date &consumeDate, const Date &checkDate) const;

public:
    CreditAccount(int id, char type, const std::string &name, double creditAmount, int repDay, const Date &openDate, const std::string &pwd, bool isShared);
    bool deposit(const Date &date, double amount) override;
    bool withdraw(const Date &date, double amount) override;
    bool consume(const Date &date, double amount);
    void updateCreditInterest(const Date &targetDate);
    double getCredit() const { return credit; }
    int getRepaymentDay() const { return repaymentDay; }
    double getCashDebt() const { return cash_debt; }
    double getConsumeDebt() const { return consume_debt + consume_debt_overdue; }
    double getConsumeDebtGrace() const { return consume_debt; }
    double getConsumeDebtOverdue() const { return consume_debt_overdue; }
    bool modifyCredit(double newCredit);
    bool modifyRepaymentDay(int newDay);
};
