#include "ui.h"

int main() {
    BankSystem bank;
    BankUI ui(bank);
    ui.run();
    return 0;
}
