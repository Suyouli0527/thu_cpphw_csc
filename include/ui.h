#pragma once
#include "banksystem.h"
#include "command.h"
#include <string>

class BankUI {
private:
    BankSystem& system;
    Command cmd;
    enum class Mode { NONE, ADMIN, USER, CARD };
    Mode currentMode;
    int insertedCardId;

    void showMainMenu();
    void adminLoop();
    void userLoop();
    void cardLoop();
    void showHelp(Mode mode) const;
    std::string getPasswordInput(const std::string &prompt) const;
    void feedback(const std::string &action, bool ok) const;
    bool executeCommand(const std::string &cmd);

public:
    BankUI(BankUI&&) = default;
    BankUI(BankSystem& sys);
    void run();
};
