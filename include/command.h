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

        if (cmd == "OPEN") {
            int id;
            char type;
            std::string accountName;
            double balance;
            iss >> id >> type >> accountName >> balance;
            System.openAccount(id, type, accountName, balance);
        } else if (cmd == "CLOSE") {
            int id;
            iss >> id;
            System.closeAccount(id);
        } else if (cmd == "MODIFYNAME") {
            int id;
            std::string username;
            iss >> id >> username;
            System.modifyName(id, username);
        } else if (cmd == "MODIFYCREDIT") {
            int id;
            double newCredit;
            iss >> id >> newCredit;
            System.modifyCredit(id, newCredit);
        } else if (cmd == "QUERY") {
            int id;
            iss >> id;
            System.query(id);
        } else if (cmd == "QUERYALL") {
            System.queryAllAccounts();
        } else if (cmd == "DEPOSIT") {
            int id;
            double amount;
            iss >> id >> amount;
            System.deposit(id, amount);
        } else if (cmd == "WITHDRAW") {
            int id;
            double amount;
            iss >> id >> amount;
            System.withdraw(id, amount);
        } else if (cmd == "TRANSFER") {
            int srcId, dstId;
            double amount;
            iss >> srcId >> dstId >> amount;
            System.transfer(srcId, dstId, amount);
        } else if (cmd == "SHOWDATE") {
            System.showDate();
        } else if (cmd == "ADD_DAYS") {
            int days;
            iss >> days;
            System.addDays(days);
        } else if (cmd == "SETDATE") {
            int year, month, day;
            iss >> year >> month >> day;
            System.setDate(year, month, day);
        } else if (cmd == "CREATEUSER") {
            std::string username;
            iss >> username;
            System.createUser(username);
        } else if (cmd == "DELETEUSER") {
            std::string username;
            iss >> username;
            System.deleteUser(username);
        } else if (cmd == "QUERYUSER") {
            std::string username;
            iss >> username;
            System.queryUser(username);
        } else if (cmd == "QUERYALLUSER") {
             System.queryAllUser();
        } else if (cmd == "SWITCHUSER") {
            std::string username;
            iss >> username;
            System.switchUser(username);
        } else if (cmd == "WHOAMI") {
            System.whoami();
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
        }
    }
    virtual ~Command() = default;
};