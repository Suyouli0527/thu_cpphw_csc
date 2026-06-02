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

    std::string getPasswordInput(const std::string &prompt) const;
    void feedback(const std::string &action, bool ok) const;

    void adminModeLoop();
    void userModeLoop();
    void cardModeLoop();

    void handleAccount();
    void handleDepositWithdraw();
    void handleFixedDeposit();
    void handleCredit();
    void handleModify();
    void handleShared();
    void handleUserMgmt();
    void handleDateLog();

    std::string collectOpen();
    std::string collectClose();
    std::string collectQuery();
    std::string collectDeposit();
    std::string collectWithdraw();
    std::string collectTransfer();
    std::string collectFixedDeposit();
    std::string collectFixedWithdraw();
    std::string collectConsume();
    std::string collectCashAdvance();
    std::string collectModifyName();
    std::string collectModifyCredit();
    std::string collectModifyShared();
    std::string collectModifyAccountPassword();
    std::string collectAddOwner();
    std::string collectRemoveOwner();
    std::string collectCreateUser();
    std::string collectDeleteUser();
    std::string collectQueryUser();
    std::string collectSwitch();
    std::string collectModifyUserPassword();
    std::string collectAddDay();
    std::string collectSetDate();
    std::string collectRollback();
    std::string collectSave();
    std::string collectResume();
    void executeAndFeedback(const std::string &commandStr);
    std::string promptLine(const std::string &label) const;
    bool promptInt(const std::string &label, int &out) const;
    bool promptDouble(const std::string &label, double &out) const;
    std::string injectCardId(const std::string &cmdStr) const;

public:
    BankUI(BankSystem& sys);
    void run();
};
