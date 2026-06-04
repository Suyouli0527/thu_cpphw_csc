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

    bool deposit(int id, double amount);
    bool withdraw(int id, double amount);
    bool transfer(int srcId, int dstId, double amount);
};
