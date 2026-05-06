#include "command.h"
#include <iostream>
#include <string>

int main() {
    BankSystem bank;
    Command cmd(bank);
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        cmd.execute(line);
    }
    return 0;
}