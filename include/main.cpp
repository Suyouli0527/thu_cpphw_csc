#include "banksystem.h"
#include <iostream>
#include <string>

int main() {
    BankSystem bank;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        bank.Command(line);
    }
    return 0;
}