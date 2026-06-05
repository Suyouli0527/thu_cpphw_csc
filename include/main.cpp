#include "command.h"
#include <iostream>
#include <string>

int main() {
    BankSystem bank;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::string ignored;
        if (!Command::execute(bank, line, ignored)) {
}
    }
    return 0;
}
