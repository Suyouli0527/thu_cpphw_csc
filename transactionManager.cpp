#include "transactionManager.h"
#include "accountManager.h"
#include "userManager.h"
#include "dateManager.h"

TransactionManager::TransactionManager(AccountManager* am, UserManager* um, DateManager* dm)
    : accountMgr(am), userMgr(um), dateMgr(dm) {}

bool TransactionManager::deposit(int id, double amount) {
    Account* acc = accountMgr->findAccount(id);
    if (!acc || !userMgr->ownsAccount(id)) return false;
    return acc->deposit(dateMgr->getCurrentDate(), amount);
}

bool TransactionManager::withdraw(int id, double amount) {
    Account* acc = accountMgr->findAccount(id);
    if (!acc || !userMgr->ownsAccount(id)) return false;
    return acc->withdraw(dateMgr->getCurrentDate(), amount);
}

bool TransactionManager::transfer(int srcId, int dstId, double amount) {
    if (srcId == dstId) return false;
    Account* srcAcc = accountMgr->findAccount(srcId);
    Account* dstAcc = accountMgr->findAccount(dstId);
    if (!srcAcc || !dstAcc) return false;
    if (!userMgr->ownsAccount(srcId)) return false;
    if (!srcAcc->withdraw(dateMgr->getCurrentDate(), amount)) return false;
    dstAcc->deposit(dateMgr->getCurrentDate(), amount);
    return true;
}
