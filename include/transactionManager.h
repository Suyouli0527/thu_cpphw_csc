#pragma once

class AccountManager;
class UserManager;
class DateManager;

class TransactionManager {
private:
    AccountManager* accountMgr;
    UserManager* userMgr;
    DateManager* dateMgr;

public:
    TransactionManager(AccountManager* am, UserManager* um, DateManager* dm);

    bool deposit(int id, double amount, int accountPassword);
    bool withdraw(int id, double amount, int accountPassword);
    bool transfer(int srcId, int dstId, double amount, int srcAccountPassword);
    bool fixedDeposit(int id, double amount, int months, int accountPassword);
    bool fixedWithdraw(int id, double amount, int accountPassword);
    bool consume(int id, double amount, int accountPassword);
    bool cashAdvance(int id, double amount, int accountPassword);
};
