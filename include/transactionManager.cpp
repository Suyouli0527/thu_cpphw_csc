#include "transactionManager.h"
#include "accountManager.h"
#include "userManager.h"
#include "dateManager.h"

TransactionManager::TransactionManager(AccountManager* am, UserManager* um, DateManager* dm)
    : accountMgr(am), userMgr(um), dateMgr(dm) {}

bool TransactionManager::deposit(int id, double amount, int accountPassword) {
    Account* acc = accountMgr->findAccount(id);
    if (!acc || !userMgr->ownsAccount(id)) return false;
    if (!acc->verifyAccountPassword(accountPassword)) return false;
    return acc->deposit(dateMgr->getCurrentDate(), amount);
}

bool TransactionManager::withdraw(int id, double amount, int accountPassword) {
    Account* acc = accountMgr->findAccount(id);
    if (!acc || !userMgr->ownsAccount(id)) return false;
    if (!acc->verifyAccountPassword(accountPassword)) return false;
    return acc->withdraw(dateMgr->getCurrentDate(), amount);
}

bool TransactionManager::transfer(int srcId, int dstId, double amount, int srcAccountPassword) {
    if (srcId == dstId) return false;
    Account* srcAcc = accountMgr->findAccount(srcId);
    Account* dstAcc = accountMgr->findAccount(dstId);
    if (!srcAcc || !dstAcc) return false;
    if (!userMgr->ownsAccount(srcId)) return false;
    if (!srcAcc->verifyAccountPassword(srcAccountPassword)) return false;
    if (!srcAcc->withdraw(dateMgr->getCurrentDate(), amount)) return false;
    dstAcc->deposit(dateMgr->getCurrentDate(), amount);
    return true;
}

bool TransactionManager::fixedDeposit(int id, double amount, int months, int accountPassword) {
    Account* acc = accountMgr->findAccount(id);
    if (!acc || !userMgr->ownsAccount(id)) return false;
    if (!acc->verifyAccountPassword(accountPassword)) return false;
    if (acc->getType() != 'S') return false;
    SavingAccount* savingAcc = static_cast<SavingAccount*>(acc);
    return savingAcc->fixedDeposit(dateMgr->getCurrentDate(), amount, months);
}

bool TransactionManager::fixedWithdraw(int id, double amount, int accountPassword) {
    Account* acc = accountMgr->findAccount(id);
    if (!acc || !userMgr->ownsAccount(id)) return false;
    if (!acc->verifyAccountPassword(accountPassword)) return false;
    if (acc->getType() != 'S') return false;
    SavingAccount* savingAcc = static_cast<SavingAccount*>(acc);
    return savingAcc->fixedWithdraw(dateMgr->getCurrentDate(), amount);
}

bool TransactionManager::consume(int id, double amount, int accountPassword) {
    Account* acc = accountMgr->findAccount(id);
    if (!acc || !userMgr->ownsAccount(id)) return false;
    if (!acc->verifyAccountPassword(accountPassword)) return false;
    if (acc->getType() != 'C') return false;
    CreditAccount* creditAcc = static_cast<CreditAccount*>(acc);
    return creditAcc->consume(dateMgr->getCurrentDate(), amount);
}

bool TransactionManager::cashAdvance(int id, double amount, int accountPassword) {
    Account* acc = accountMgr->findAccount(id);
    if (!acc || !userMgr->ownsAccount(id)) return false;
    if (!acc->verifyAccountPassword(accountPassword)) return false;
    if (acc->getType() != 'C') return false;
    CreditAccount* creditAcc = static_cast<CreditAccount*>(acc);
    return creditAcc->cashAdvance(dateMgr->getCurrentDate(), amount);
}
