#include "command.h"
#include "tools.h"
#include <iostream>
#include <string>

int main() {
    BankSystem bank;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        if (!Command::execute(bank, line)) {
            Tools::printFailure();
        }
    }
    return 0;
}
