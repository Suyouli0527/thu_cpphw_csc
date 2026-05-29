#pragma once
#include "banksystem.h"
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

class Command {
private:
    BankSystem& system;

    static bool isPositiveInt(const std::string& s) {
        if (s.empty()) return false;
        for (char c : s) {
            if (c < '0' || c > '9') return false;
        }
        return true;
    }

public:
    Command(BankSystem &sys) : system(sys) {}

    void execute(const std::string &command) {
        system.setRawCommand(command);
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        if (iss.fail()) { Tools::printFailure(); return; }

        if (cmd == "OPEN") {
            int id; char type; std::string name; double balance;
            if (!(iss >> id >> type >> name >> balance)) {
                Tools::printFailure(); return;
            }
            int repaymentDay = 0;
            if (type == 'C') {
                if (!(iss >> repaymentDay)) { Tools::printFailure(); return; }
            }
            int accountPassword, sharedInt;
            if (!(iss >> accountPassword >> sharedInt)) { Tools::printFailure(); return; }
            system.openAccount(id, type, name, balance, repaymentDay, accountPassword, sharedInt != 0);
        }
        else if (cmd == "CLOSE") {
            int id, accountPassword;
            if (!(iss >> id >> accountPassword)) { Tools::printFailure(); return; }
            system.closeAccount(id, accountPassword);
        }
        else if (cmd == "MODIFY") {
            std::string subCmd;
            if (!(iss >> subCmd)) { Tools::printFailure(); return; }
            if (subCmd == "NAME") {
                int id, accountPassword; std::string newName;
                if (!(iss >> id >> newName >> accountPassword)) { Tools::printFailure(); return; }
                system.modifyName(id, newName, accountPassword);
            } else if (subCmd == "CREDIT") {
                int id, accountPassword; double newCredit;
                if (!(iss >> id >> newCredit >> accountPassword)) { Tools::printFailure(); return; }
                system.modifyCredit(id, newCredit, accountPassword);
            } else {
                Tools::printFailure();
            }
        }
        else if (cmd == "MODIFY_USERPASSWORD") {
            std::string oldPassword, newPassword;
            if (!(iss >> oldPassword >> newPassword)) { Tools::printFailure(); return; }
            system.changeUserPassword(oldPassword, newPassword);
        }
        else if (cmd == "MODIFY_ACCOUNTPASSWORD") {
            int id, oldPassword, newPassword;
            if (!(iss >> id >> oldPassword >> newPassword)) { Tools::printFailure(); return; }
            system.changeAccountPassword(id, oldPassword, newPassword);
        }
        else if (cmd == "ADD_OWNER") {
            int id; std::string userName;
            if (!(iss >> id >> userName)) { Tools::printFailure(); return; }
            system.addOwner(id, userName);
        }
        else if (cmd == "REMOVE_OWNER") {
            int id; std::string userName;
            if (!(iss >> id >> userName)) { Tools::printFailure(); return; }
            system.removeOwner(id, userName);
        }
        else if (cmd == "QUERY") {
            int id, accountPassword;
            if (!(iss >> id >> accountPassword)) { Tools::printFailure(); return; }
            system.query(id, accountPassword);
        }
        else if (cmd == "QUERYALL") {
            system.queryAllAccounts();
        }
        else if (cmd == "DEPOSIT") {
            int id, accountPassword; double amount;
            if (!(iss >> id >> amount >> accountPassword)) { Tools::printFailure(); return; }
            system.deposit(id, amount, accountPassword);
        }
        else if (cmd == "WITHDRAW") {
            int id, accountPassword; double amount;
            if (!(iss >> id >> amount >> accountPassword)) { Tools::printFailure(); return; }
            system.withdraw(id, amount, accountPassword);
        }
        else if (cmd == "CONSUME") {
            int id, accountPassword; double amount;
            if (!(iss >> id >> amount >> accountPassword)) { Tools::printFailure(); return; }
            system.consume(id, amount, accountPassword);
        }
        else if (cmd == "CASH_ADVANCE") {
            int id, accountPassword; double amount;
            if (!(iss >> id >> amount >> accountPassword)) { Tools::printFailure(); return; }
            system.cashAdvance(id, amount, accountPassword);
        }
        else if (cmd == "TRANSFER") {
            int srcId, dstId, accountPassword; double amount;
            if (!(iss >> srcId >> dstId >> amount >> accountPassword)) { Tools::printFailure(); return; }
            system.transfer(srcId, dstId, amount, accountPassword);
        }
        else if (cmd == "FIXED_DEPOSIT") {
            int id, accountPassword; double amount; int months;
            if (!(iss >> id >> amount >> months >> accountPassword)) { Tools::printFailure(); return; }
            system.fixedDeposit(id, amount, months, accountPassword);
        }
        else if (cmd == "FIXED_WITHDRAW") {
            int id, accountPassword; double amount;
            if (!(iss >> id >> amount >> accountPassword)) { Tools::printFailure(); return; }
            system.fixedWithdraw(id, amount, accountPassword);
        }
        else if (cmd == "SHOW_DATE") {
            system.showDate();
        }
        else if (cmd == "ADD_DAY") {
            std::string daysStr;
            if (!(iss >> daysStr)) { Tools::printFailure(); return; }
            if (!isPositiveInt(daysStr)) { Tools::printFailure(); return; }
            int days = std::stoi(daysStr);
            if (days > 0) {
                system.addDays(days);
            } else {
                Tools::printFailure();
            }
        }
        else if (cmd == "SET_DATE") {
            std::string yStr, mStr, dStr;
            if (!(iss >> yStr >> mStr >> dStr)) { Tools::printFailure(); return; }
            if (!isPositiveInt(yStr) || !isPositiveInt(mStr) || !isPositiveInt(dStr)) {
                Tools::printFailure(); return;
            }
            int year = std::stoi(yStr);
            int month = std::stoi(mStr);
            int day = std::stoi(dStr);
            system.setDate(year, month, day);
        }
        else if (cmd == "CREATE_USER") {
            std::string username, password;
            if (!(iss >> username >> password)) { Tools::printFailure(); return; }
            system.createUser(username, password);
        }
        else if (cmd == "DELETE_USER") {
            std::string username;
            if (!(iss >> username)) { Tools::printFailure(); return; }
            system.deleteUser(username);
        }
        else if (cmd == "QUERY_USER") {
            std::string username;
            if (!(iss >> username)) { Tools::printFailure(); return; }
            system.queryUser(username);
        }
        else if (cmd == "QUERY_USERLIST") {
            system.queryAllUser();
        }
        else if (cmd == "SWITCH") {
            std::string username, password;
            if (!(iss >> username >> password)) { Tools::printFailure(); return; }
            system.switchUser(username, password);
        }
        else if (cmd == "WHOAMI") {
            system.whoami();
        }
        else if (cmd == "LOG") {
            system.showLog();
        }
        else if (cmd == "ROLLBACK") {
            std::string idStr;
            if (!(iss >> idStr)) { Tools::printFailure(); return; }
            if (!isPositiveInt(idStr) && idStr != "0") {
                Tools::printFailure(); return;
            }
            int n = std::stoi(idStr);
            system.rollback(n);
        }
        else if (cmd == "SAVE") {
            std::string filename;
            if (!(iss >> filename)) { Tools::printFailure(); return; }
            system.saveLog(filename);
        }
        else if (cmd == "RESUME") {
            std::string filename;
            if (!(iss >> filename)) { Tools::printFailure(); return; }
            system.resume(filename);
        }
        else {
            Tools::printFailure();
        }
    }
};
