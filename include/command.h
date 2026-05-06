#pragma once
#include "banksystem.h"
#include "tools.h"
#include <sstream>
#include <string>


class Command {
private:
    BankSystem& System;
public:
    Command(BankSystem& sys):System(sys) {};
    virtual void execute(const std::string& command){
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "open") {
            int id;
            char type;
            std::string accountName;
            double balance;
            iss >> id >> type >> accountName >> balance;
            System.openAccount(id, type, accountName, balance);
        } else if (cmd == "close") {
            int id;
            iss >> id;
            System.closeAccount(id);
        } else if (cmd == "modifyname") {
            int id;
            std::string username;
            iss >> id >> username;
            System.modifyName(id, username);
        } else if (cmd == "modifycredit") {
            int id;
            double newCredit;
            iss >> id >> newCredit;
            System.modifyCredit(id, newCredit);
        } else if (cmd == "query") {
            int id;
            iss >> id;
            System.query(id);
        } else if (cmd == "queryallaccounts") {
            System.queryallAccounts();
        } else if (cmd == "deposit") {
            int id;
            double amount;
            iss >> id >> amount;
            System.deposit(id, amount);
        } else if (cmd == "withdraw") {
            int id;
            double amount;
            iss >> id >> amount;
            System.withdraw(id, amount);
        } else if (cmd == "transfer") {
            int srcId, dstId;
            double amount;
            iss >> srcId >> dstId >> amount;
            System.transfer(srcId, dstId, amount);
        } else if (cmd == "showdate") {
            System.showDate();
        } else if (cmd == "adddays") {
            int days;
            iss >> days;
            System.addDays(days);
        } else if (cmd == "setdate") {
            int year, month, day;
            iss >> year >> month >> day;
            System.setDate(year, month, day);
        } else if (cmd == "createuser") {
            std::string username;
            iss >> username;
            System.createUser(username);
        } else if (cmd == "deleteuser") {
            std::string username;
            iss >> username;
            System.deleteUser(username);
        } else if (cmd == "queryuser") {
            std::string username;
            iss >> username;
            System.queryUser(username);
        } else if (cmd == "queryalluser") {
             System.queryAllUser();
        } else if (cmd == "switchuser") {
            std::string username;
            iss >> username;
            System.switchUser(username);
        } else if (cmd == "whoami") {
            System.whoami();
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
        }
    }
    virtual ~Command() = default;
};