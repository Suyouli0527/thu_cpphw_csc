#include <iostream>
#include <iomanip>
#include "account.h"
#include "date.h"

int main() {
    // 初始化日期与账户
    Date startDate(2026, 1, 1);
    SavingAccount sa(1001, 's', "Savings", 10000.0, startDate);
    CreditAccount ca(2001, 'c', "Credit", 5000.0, startDate);  // 信用额度5000

    // 信用账户透支 2000 元，余额为负
    ca.withdraw(startDate, 2000.0);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "初始状态:\n";
    std::cout << "  Savings balance: " << sa.getBalance() 
              << ", interest: " << sa.getInterest() << "\n";
    std::cout << "  Credit balance: " << ca.getBalance() 
              << ", interest: " << ca.getInterest() << "\n\n";

    // 推进日期到 2 月 2 日（跨越 1 月 31 天 + 2 月 1 天 = 32 天）
    Date future(2026, 2, 2);
    sa.updateInterest(future);
    ca.updateInterest(future);

    std::cout << "经过 32 天（1月1日→2月2日）后:\n";
    std::cout << "  Savings balance: " << sa.getBalance() 
              << ", interest: " << sa.getInterest() << "\n";
    std::cout << "  Credit balance: " << ca.getBalance() 
              << ", interest: " << ca.getInterest() << "\n";

    return 0;
}