#pragma once
#include "banksystem.h"
#include "tools.h"
#include <sstream>
#include <fstream>
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
        }
         else if (cmd == "CLOSE") {
            int id;
            iss >> id;
            System.closeAccount(id);
        }
         else if (cmd == "MODIFY") {
            std::string subCmd;
            iss >> subCmd;
            if (subCmd == "NAME") {
                int id;
                std::string newName;
                iss >> id >> newName;
                System.modifyName(id, newName);
            } else if (subCmd == "CREDIT") {
                int id;
                double newCredit;
                iss >> id >> newCredit;
                System.modifyCredit(id, newCredit);
            } else {
                Tools::printFailure();
            }
        }
         else if (cmd == "QUERY") {
            int id;
            iss >> id;
            System.query(id);
        } 
        else if (cmd == "QUERYALL") {
            System.queryAllAccounts();
        } 
        else if (cmd == "DEPOSIT") {
            int id;
            double amount;
            iss >> id >> amount;
            System.deposit(id, amount);
        } 
        else if (cmd == "WITHDRAW") {
            int id;
            double amount;
            iss >> id >> amount;
            System.withdraw(id, amount);
        } 
        else if (cmd == "TRANSFER") {
            int srcId, dstId;
            double amount;
            iss >> srcId >> dstId >> amount;
            System.transfer(srcId, dstId, amount);
        } 
        else if (cmd == "SHOW_DATE") {
            System.showDate();
        } 
        else if (cmd == "ADD_DAY") {
            double days;
            iss >> days;
            if(days ==static_cast<int>(days)  ){
                System.addDays(static_cast<int>(days));
            } 
            else {
                Tools::printFailure();
            }
        } 
        else if (cmd == "SET_DATE") {
            int year, month, day;
            iss >> year >> month >> day;
            System.setDate(year, month, day);
        } 
        else if (cmd == "CREATE_USER") {
            std::string username;
            iss >> username;
            System.createUser(username);
        } 
        else if (cmd == "DELETE_USER") {
            std::string username;
            iss >> username;
            System.deleteUser(username);
        } 
        else if (cmd == "QUERY_USER") {
            std::string username;
            iss >> username;
            System.queryUser(username);
        } 
        else if (cmd == "QUERY_USERLIST") {
             System.queryAllUser();
        } 
        else if (cmd == "SWITCH") {
            std::string username;
            iss >> username;
            System.switchUser(username);
        } 
        else if (cmd == "WHOAMI") {
            System.whoami();
        } 
    
        else if(cmd=="LOG"){
            System.showLog();
        }
        
        else if(cmd=="ROLLBACK"){
            int n;
            iss>>n;
            System.rollback(n);
        }
        else if(cmd=="SAVE"){
            std::string filename;
            iss>>filename;
            System.SaveLog(filename);
        }
         else if (cmd == "RESUME") {
            std::string filename;
            iss >> filename;
            if (!System.isInitialState()) {
            Tools::printFailure();
            return;
         }
            std::ifstream file(filename);
            if (!file.is_open()) {
            Tools::printFailure();
            return;
        }
            std::vector<std::string> subCommands;
            std::string line;
            while (std::getline(file, line)) {
            if (line.empty()) continue;
            auto pos = line.find(' ');
            if (pos == std::string::npos) {
            Tools::printFailure();
            return;
        }
        subCommands.push_back(line.substr(pos + 1));
        }
        file.close();
        System.setSilent(true);
        for (const auto &cmd : subCommands) {
            execute(cmd);
        }
        System.setSilent(false);
        Tools::printSuccess();
}

        else {
            Tools::printFailure();
        }
        
    }
    virtual ~Command() = default;
};