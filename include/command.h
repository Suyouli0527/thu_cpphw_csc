#pragma once
#include "banksystem.h"
#include <sstream>
#include <string>
#include <vector>

class Command {
private:
    BankSystem& system;
    std::string m_lastAction;

    static bool isPositiveInt(const std::string& s) {
        if (s.empty()) return false;
        for (char c : s) {
            if (c < '0' || c > '9') return false;
        }
        return true;
    }

public:
    Command(BankSystem &sys) : system(sys) {}

    const std::string& lastAction() const { return m_lastAction; }

    bool execute(const std::string &command) {
        system.setRawCommand(command);
        system.resetResult();
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        if (iss.fail()) return false;
        m_lastAction = cmd;

        if (cmd == "OPEN") {
            int id; char type; std::string name; double balance;
            if (!(iss >> id >> type >> name >> balance)) return false;
            int repaymentDay = 0;
            if (type == 'C') {
                if (!(iss >> repaymentDay)) return false;
            }
            int accountPassword, sharedInt;
            if (!(iss >> accountPassword >> sharedInt)) return false;
            system.openAccount(id, type, name, balance, repaymentDay, accountPassword, sharedInt != 0);
        }
        else if (cmd == "CLOSE") {
            int id, accountPassword;
            if (!(iss >> id >> accountPassword)) return false;
            system.closeAccount(id, accountPassword);
        }
        else if (cmd == "MODIFY") {
            std::string subCmd;
            if (!(iss >> subCmd)) return false;
            if (subCmd == "NAME") {
                int id, accountPassword; std::string newName;
                if (!(iss >> id >> newName >> accountPassword)) return false;
                system.modifyName(id, newName, accountPassword);
            } else if (subCmd == "CREDIT") {
                int id, accountPassword; double newCredit;
                if (!(iss >> id >> newCredit >> accountPassword)) return false;
                system.modifyCredit(id, newCredit, accountPassword);
            } else if (subCmd == "SHARED") {
                int id; std::string sharedStr;
                if (!(iss >> id >> sharedStr)) return false;
                bool shared;
                if (sharedStr == "ys") shared = true;
                else if (sharedStr == "ns") shared = false;
                else return false;
                system.modifyShared(id, shared);
            } else return false;
        }
        else if (cmd == "MODIFY_USERPASSWORD") {
            std::string oldPassword, newPassword;
            if (!(iss >> oldPassword >> newPassword)) return false;
            system.changeUserPassword(oldPassword, newPassword);
        }
        else if (cmd == "MODIFY_ACCOUNTPASSWORD") {
            int id, oldPassword, newPassword;
            if (!(iss >> id >> oldPassword >> newPassword)) return false;
            system.changeAccountPassword(id, oldPassword, newPassword);
        }
        else if (cmd == "ADD_OWNER") {
            int id; std::string userName;
            if (!(iss >> id >> userName)) return false;
            system.addOwner(id, userName);
        }
        else if (cmd == "REMOVE_OWNER") {
            int id; std::string userName;
            if (!(iss >> id >> userName)) return false;
            system.removeOwner(id, userName);
        }
        else if (cmd == "QUERY") {
            int id, accountPassword;
            if (!(iss >> id >> accountPassword)) return false;
            system.query(id, accountPassword);
        }
        else if (cmd == "QUERYALL") {
            system.queryAllAccounts();
        }
        else if (cmd == "DEPOSIT") {
            int id, accountPassword; double amount;
            if (!(iss >> id >> amount >> accountPassword)) return false;
            system.deposit(id, amount, accountPassword);
        }
        else if (cmd == "WITHDRAW") {
            int id, accountPassword; double amount;
            if (!(iss >> id >> amount >> accountPassword)) return false;
            system.withdraw(id, amount, accountPassword);
        }
        else if (cmd == "CONSUME") {
            int id, accountPassword; double amount;
            if (!(iss >> id >> amount >> accountPassword)) return false;
            system.consume(id, amount, accountPassword);
        }
        else if (cmd == "CASH_ADVANCE") {
            int id, accountPassword; double amount;
            if (!(iss >> id >> amount >> accountPassword)) return false;
            system.cashAdvance(id, amount, accountPassword);
        }
        else if (cmd == "TRANSFER") {
            int srcId, dstId, accountPassword; double amount;
            if (!(iss >> srcId >> dstId >> amount >> accountPassword)) return false;
            system.transfer(srcId, dstId, amount, accountPassword);
        }
        else if (cmd == "FIXED_DEPOSIT") {
            int id, accountPassword; double amount; int months;
            if (!(iss >> id >> amount >> months >> accountPassword)) return false;
            system.fixedDeposit(id, amount, months, accountPassword);
        }
        else if (cmd == "FIXED_WITHDRAW") {
            int id, accountPassword; double amount;
            if (!(iss >> id >> amount >> accountPassword)) return false;
            system.fixedWithdraw(id, amount, accountPassword);
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
            std::string username, password;
            if (!(iss >> username >> password)) return false;
            system.createUser(username, password);
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
            std::string username, password;
            if (!(iss >> username >> password)) return false;
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
