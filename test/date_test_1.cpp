#include "date.h"
#include <iostream>

int main() {
    // 测试1: 默认构造 1970-1-1
    Date d1;
    std::cout << "Test 1: ";
    d1.showDate();
    std::cout << " [Expected: Thursday, January 1, 1970]" << std::endl;

    // 测试2: 闰年2月28日 +1天 → 2月29日
    Date d2(2024, 2, 28);
    d2.addDays(1);
    std::cout << "Test 2: ";
    d2.showDate();
    std::cout << " [Expected: Thursday, February 29, 2024]" << std::endl;

    // 测试3: 闰年2月28日 +2天 → 跨到3月1日
    Date d3(2024, 2, 28);
    d3.addDays(2);
    std::cout << "Test 3: ";
    d3.showDate();
    std::cout << " [Expected: Friday, March 1, 2024]" << std::endl;

    // 测试4: 跨年 12月31日 +1天 → 次年1月1日
    Date d4(2024, 12, 31);
    d4.addDays(1);
    std::cout << "Test 4: ";
    d4.showDate();
    std::cout << " [Expected: Wednesday, January 1, 2025]" << std::endl;

    // 测试5: 平年整年 365天 → 日期不变但年份+1
    Date d5(2023, 1, 1);
    d5.addDays(365);
    std::cout << "Test 5: ";
    d5.showDate();
    std::cout << " [Expected: Monday, January 1, 2024]" << std::endl;

    return 0;
}