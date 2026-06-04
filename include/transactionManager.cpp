#include "transactionManager.h"
#include "accountManager.h"
#include "userManager.h"
#include "dateManager.h"

TransactionManager::TransactionManager(AccountManager* am, UserManager* um, DateManager* dm)
    : accountMgr(am), userMgr(um), dateMgr(dm) {}

Account* TransactionManager::validateAndGet(int id, int accountPassword) {
    Account* acc = accountMgr->findAccount(id);
    if (!acc || !userMgr->ownsAccount(id)) return nullptr;
    if (!acc->verifyAccountPassword(accountPassword)) return nullptr;
    return acc;
}

bool TransactionManager::deposit(int id, double amount, int accountPassword) {
    Account* acc = validateAndGet(id, accountPassword);
    if (!acc) return false;
    return acc->deposit(dateMgr->getCurrentDate(), amount);
}

bool TransactionManager::withdraw(int id, double amount, int accountPassword) {
    Account* acc = validateAndGet(id, accountPassword);
    if (!acc) return false;
    return acc->withdraw(dateMgr->getCurrentDate(), amount);
}

bool TransactionManager::transfer(int srcId, int dstId, double amount, int srcAccountPassword) {
    if (srcId == dstId) return false;
    Account* srcAcc = validateAndGet(srcId, srcAccountPassword);
    if (!srcAcc) return false;
    Account* dstAcc = accountMgr->findAccount(dstId);
    if (!dstAcc) return false;
    if (!srcAcc->withdraw(dateMgr->getCurrentDate(), amount)) return false;
    dstAcc->deposit(dateMgr->getCurrentDate(), amount);
    return true;
}

bool TransactionManager::fixedDeposit(int id, double amount, int months, int accountPassword) {
    Account* acc = validateAndGet(id, accountPassword);
    if (!acc || acc->getType() != 'S') return false;
    SavingAccount* savingAcc = static_cast<SavingAccount*>(acc);
    return savingAcc->fixedDeposit(dateMgr->getCurrentDate(), amount, months);
}

bool TransactionManager::fixedWithdraw(int id, double amount, int accountPassword) {
    Account* acc = validateAndGet(id, accountPassword);
    if (!acc || acc->getType() != 'S') return false;
    SavingAccount* savingAcc = static_cast<SavingAccount*>(acc);
    return savingAcc->fixedWithdraw(dateMgr->getCurrentDate(), amount);
}

bool TransactionManager::consume(int id, double amount, int accountPassword) {
    Account* acc = validateAndGet(id, accountPassword);
    if (!acc || acc->getType() != 'C') return false;
    CreditAccount* creditAcc = static_cast<CreditAccount*>(acc);
    return creditAcc->consume(dateMgr->getCurrentDate(), amount);
}
