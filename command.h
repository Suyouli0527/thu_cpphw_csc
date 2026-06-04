#pragma once
#include "banksystem.h"
#include <sstream>
#include <string>
#include <vector>

class Command {
private:
    static bool isPositiveInt(const std::string& s) {
        if (s.empty()) return false;
        for (char c : s) {
            if (c < '0' || c > '9') return false;
        }
        return true;
    }

public:
    static bool execute(BankSystem &system, const std::string &command) {
        system.setRawCommand(command);
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        if (iss.fail()) return false;

        if (cmd == "OPEN") {
            int id; char type; std::string name; double balance;
            if (!(iss >> id >> type >> name >> balance)) return false;
            system.openAccount(id, type, name, balance);
        }
        else if (cmd == "CLOSE") {
            int id;
            if (!(iss >> id)) return false;
            system.closeAccount(id);
        }
        else if (cmd == "MODIFY") {
            std::string subCmd;
            if (!(iss >> subCmd)) return false;
            if (subCmd == "NAME") {
                int id; std::string newName;
                if (!(iss >> id >> newName)) return false;
                system.modifyName(id, newName);
            } else if (subCmd == "CREDIT") {
                int id; double newCredit;
                if (!(iss >> id >> newCredit)) return false;
                system.modifyCredit(id, newCredit);
            } else return false;
        }
        else if (cmd == "QUERY") {
            int id;
            if (!(iss >> id)) return false;
            system.query(id);
        }
        else if (cmd == "QUERYALL") {
            system.queryAllAccounts();
        }
        else if (cmd == "DEPOSIT") {
            int id; double amount;
            if (!(iss >> id >> amount)) return false;
            system.deposit(id, amount);
        }
        else if (cmd == "WITHDRAW") {
            int id; double amount;
            if (!(iss >> id >> amount)) return false;
            system.withdraw(id, amount);
        }
        else if (cmd == "TRANSFER") {
            int srcId, dstId; double amount;
            if (!(iss >> srcId >> dstId >> amount)) return false;
            system.transfer(srcId, dstId, amount);
        }
        else if (cmd == "SHOW_DATE") {
            system.showDate();
        }
        else if (cmd == "ADD_DAY") {
            std::string daysStr;
            if (!(iss >> daysStr)) return false;
            if (!isPositiveInt(daysStr)) return false;
            int days = std::stoi(daysStr);
            if (days <= 0) return false;
            system.addDays(days);
        }
        else if (cmd == "SET_DATE") {
            std::string yStr, mStr, dStr;
            if (!(iss >> yStr >> mStr >> dStr)) return false;
            if (!isPositiveInt(yStr) || !isPositiveInt(mStr) || !isPositiveInt(dStr)) return false;
            int year = std::stoi(yStr);
            int month = std::stoi(mStr);
            int day = std::stoi(dStr);
            system.setDate(year, month, day);
        }
        else if (cmd == "CREATE_USER") {
            std::string username;
            if (!(iss >> username)) return false;
            system.createUser(username);
        }
        else if (cmd == "DELETE_USER") {
            std::string username;
            if (!(iss >> username)) return false;
            system.deleteUser(username);
        }
        else if (cmd == "QUERY_USER") {
            std::string username;
            if (!(iss >> username)) return false;
            system.queryUser(username);
        }
        else if (cmd == "QUERY_USERLIST") {
            system.queryAllUser();
        }
        else if (cmd == "SWITCH") {
            std::string username;
            if (!(iss >> username)) return false;
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
            if (!(iss >> idStr)) return false;
            if (!isPositiveInt(idStr) && idStr != "0") return false;
            int n = std::stoi(idStr);
            system.rollback(n);
        }
        else if (cmd == "SAVE") {
            std::string filename;
            if (!(iss >> filename)) return false;
            system.saveLog(filename);
        }
        else if (cmd == "RESUME") {
            std::string filename;
            if (!(iss >> filename)) return false;
            system.resume(filename);
        }
        else return false;

        return true;
    }
};
