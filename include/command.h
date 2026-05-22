#pragma once
#include "banksystem.h"
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

class Command {
private:
    BankSystem& system;

public:
    Command(BankSystem &sys) : system(sys) {}

    void execute(const std::string &command) {
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
        else if (cmd == "SHOW_DATE") {
            system.showDate();
        }
        else if (cmd == "ADD_DAY") {
            double days;
            if (!(iss >> days)) { Tools::printFailure(); return; }
            if (days > 0 && days == static_cast<int>(days)) {
                system.addDays(static_cast<int>(days));
            } else {
                Tools::printFailure();
            }
        }
        else if (cmd == "SET_DATE") {
            int year, month, day;
            if (!(iss >> year >> month >> day)) { Tools::printFailure(); return; }
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
            int n;
            if (!(iss >> n)) { Tools::printFailure(); return; }
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
