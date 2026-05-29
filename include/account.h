#pragma once
#include "date.h"
#include "interestCalculator.h"
#include <string>
#include <vector>

class Account {
protected:
    int id;
    std::string name;
    char type;
    double balance;
    Date openDate;
    Date lastInterestDate;
    double interest;

public:
    Account(int id, char type, const std::string &name, double balance, const Date &openDate);
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

    bool modifyName(const std::string &newName);
};

struct FixedDeposit {
    double principal;
    int months;
    Date depositDate;
    Date maturityDate;
    bool partiallyWithdrawn;
};

class SavingAccount : public Account {
    std::vector<FixedDeposit> fixedDeposits;

public:
    SavingAccount(int id, char type, const std::string &name, double balance, const Date &openDate);
    bool deposit(const Date &date, double amount) override;
    bool withdraw(const Date &date, double amount) override;

    bool fixedDeposit(const Date &date, double amount, int months);
    bool fixedWithdraw(const Date &date, double amount);
    void updateFixedDeposits(const Date &date);
    std::vector<FixedDeposit>& getFixedDeposits() { return fixedDeposits; }
    const std::vector<FixedDeposit>& getFixedDeposits() const { return fixedDeposits; }
    int getFixedDepositCount() const { return fixedDeposits.size(); }
};

class CreditAccount : public Account {
private:
    double credit;
public:
    CreditAccount(int id, char type, const std::string &name, double creditAmount, const Date &openDate);
    bool deposit(const Date &date, double amount) override;
    bool withdraw(const Date &date, double amount) override;
    double getCredit() const { return credit; }
    bool modifyCredit(double newCredit);
};
