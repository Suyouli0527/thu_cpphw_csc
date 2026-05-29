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
            system.openAccount(id, type, name, balance);
        }
        else if (cmd == "CLOSE") {
            int id;
            if (!(iss >> id)) { Tools::printFailure(); return; }
            system.closeAccount(id);
        }
        else if (cmd == "MODIFY") {
            std::string subCmd;
            if (!(iss >> subCmd)) { Tools::printFailure(); return; }
            if (subCmd == "NAME") {
                int id; std::string newName;
                if (!(iss >> id >> newName)) { Tools::printFailure(); return; }
                system.modifyName(id, newName);
            } else if (subCmd == "CREDIT") {
                int id; double newCredit;
                if (!(iss >> id >> newCredit)) { Tools::printFailure(); return; }
                system.modifyCredit(id, newCredit);
            } else {
                Tools::printFailure();
            }
        }
        else if (cmd == "QUERY") {
            int id;
            if (!(iss >> id)) { Tools::printFailure(); return; }
            system.query(id);
        }
        else if (cmd == "QUERYALL") {
            system.queryAllAccounts();
        }
        else if (cmd == "DEPOSIT") {
            int id; double amount;
            if (!(iss >> id >> amount)) { Tools::printFailure(); return; }
            system.deposit(id, amount);
        }
        else if (cmd == "WITHDRAW") {
            int id; double amount;
            if (!(iss >> id >> amount)) { Tools::printFailure(); return; }
            system.withdraw(id, amount);
        }
        else if (cmd == "TRANSFER") {
            int srcId, dstId; double amount;
            if (!(iss >> srcId >> dstId >> amount)) { Tools::printFailure(); return; }
            system.transfer(srcId, dstId, amount);
        }
        else if (cmd == "FIXED_DEPOSIT") {
            int id; double amount; int months;
            if (!(iss >> id >> amount >> months)) { Tools::printFailure(); return; }
            system.fixedDeposit(id, amount, months);
        }
        else if (cmd == "FIXED_WITHDRAW") {
            int id; double amount;
            if (!(iss >> id >> amount)) { Tools::printFailure(); return; }
            system.fixedWithdraw(id, amount);
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
            std::string username;
            if (!(iss >> username)) { Tools::printFailure(); return; }
            system.createUser(username);
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
            std::string username;
            if (!(iss >> username)) { Tools::printFailure(); return; }
            system.switchUser(username);
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
