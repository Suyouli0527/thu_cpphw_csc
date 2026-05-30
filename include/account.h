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
    int accountPassword;
    bool shared;
    std::vector<std::string> owners;

public:
    Account(int id, char type, const std::string &name, double balance, const Date &openDate, int pwd, bool isShared);
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
    const std::vector<std::string>& getOwners() const { return owners; }
    void addOwner(const std::string &userName);
    void removeOwner(const std::string &userName);
    bool verifyAccountPassword(int pwd) const { return accountPassword == pwd; }
    bool changeAccountPassword(int oldPwd, int newPwd) {
        if (accountPassword != oldPwd) return false;
        if (newPwd < 100000 || newPwd > 999999) return false;
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
};

struct FundHolding {
    double shares;
    double navAtPurchase;
    Date purchaseDate;
    int fundIndex;
};

struct WealthProductHolding {
    double principal;
    int productIndex;
    Date purchaseDate;
    Date maturityDate;
    bool settled;
};

class SavingAccount : public Account {
    std::vector<FixedDeposit> fixedDeposits;
    std::vector<FundHolding> fundHoldings;
    std::vector<WealthProductHolding> wealthHoldings;

public:
    SavingAccount(int id, char type, const std::string &name, double balance, const Date &openDate, int pwd, bool isShared);
    bool deposit(const Date &date, double amount) override;
    bool withdraw(const Date &date, double amount) override;

    bool fixedDeposit(const Date &date, double amount, int months);
    bool fixedWithdraw(const Date &date, double amount);
    void updateFixedDeposits(const Date &date);
    std::vector<FixedDeposit>& getFixedDeposits() { return fixedDeposits; }
    const std::vector<FixedDeposit>& getFixedDeposits() const { return fixedDeposits; }
    int getFixedDepositCount() const { return (int)fixedDeposits.size(); }

    bool buyFund(const Date &date, int fundIndex, double amount);
    bool sellFund(int holdingIndex, const Date &date);
    bool buyWealthProduct(const Date &date, int productIndex, double amount);
    void updateWealthProducts(const Date &date);
    std::vector<FundHolding>& getFundHoldings() { return fundHoldings; }
    const std::vector<FundHolding>& getFundHoldings() const { return fundHoldings; }
    int getFundHoldingCount() const { return (int)fundHoldings.size(); }
    std::vector<WealthProductHolding>& getWealthHoldings() { return wealthHoldings; }
    const std::vector<WealthProductHolding>& getWealthHoldings() const { return wealthHoldings; }
    int getWealthHoldingCount() const { return (int)wealthHoldings.size(); }
};

struct CreditTransaction {
    double amount;
    char txnType; 
    Date date;
};

class CreditAccount : public Account {
private:
    double credit;
    int repaymentDay;
    std::vector<CreditTransaction> transactions;
    Date lastInterestUpdate;

    double getCashAdvanceDebtInternal() const;
    double getConsumeDebtInternal() const;
    Date getRepaymentDate(const Date &current) const;
    bool isWithinGracePeriod(const Date &txnDate, const Date &checkDate) const;

public:
    CreditAccount(int id, char type, const std::string &name, double creditAmount, int repDay, const Date &openDate, int pwd, bool isShared);
    bool deposit(const Date &date, double amount) override;
    bool withdraw(const Date &date, double amount) override;
    bool consume(const Date &date, double amount);
    bool cashAdvance(const Date &date, double amount);
    void updateCreditInterest(const Date &targetDate);
    double getCredit() const { return credit; }
    int getRepaymentDay() const { return repaymentDay; }
    double getTotalDebt() const;
    double getCashAdvanceDebt() const;
    double getConsumeDebt() const;
    const std::vector<CreditTransaction>& getTransactions() const { return transactions; }
    bool modifyCredit(double newCredit);
};
