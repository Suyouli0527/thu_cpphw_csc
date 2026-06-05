#pragma once
#include "banksystem.h"
#include <iostream>
#include <sstream>
#include <string>

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
    // 静态命令解析与执行
    // 成功时 outAction 接收命令词，返回 true；解析失败返回 false
    static bool execute(BankSystem &system, const std::string &command, std::string &outAction) {
        system.setRawCommand(command);
        system.resetResult();
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        if (iss.fail()) return false;
        outAction = cmd;

        if (cmd == "OPEN") {
            int id; char type; std::string name; double balance;
            if (!(iss >> id >> type >> name >> balance)) return false;
            int repaymentDay = 15;
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
            int id; std::string userName, levelStr; double withdrawLimit = 0;
            if (!(iss >> id >> userName >> levelStr)) return false;
            if (levelStr == "RESTRICTED") {
                if (!(iss >> withdrawLimit)) return false;
            }
            system.addOwner(id, userName, levelStr, withdrawLimit);
        }
        else if (cmd == "FREEZE") {
            int id;
            if (!(iss >> id)) return false;
            system.freezeAccount(id);
        }
        else if (cmd == "UNFREEZE") {
            int id;
            if (!(iss >> id)) return false;
            system.unfreezeAccount(id);
        }
        else if (cmd == "MODIFY_OWNER_LIMIT") {
            int id; std::string userName; double newLimit;
            if (!(iss >> id >> userName >> newLimit)) return false;
            system.modifyOwnerLimit(id, userName, newLimit);
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
        else if (cmd == "TRANSFER") {
            int srcId, dstId, accountPassword; double amount;
            if (!(iss >> srcId >> dstId >> amount >> accountPassword)) return false;
            system.transfer(srcId, dstId, amount, accountPassword);
        }
        else if (cmd == "FIXED_DEPOSIT") {
            int id, accountPassword; double amount; int months;
            if (!(iss >> id >> amount >> months >> accountPassword)) return false;
            std::string autoStr;
            bool autoRenew = false;
            if (iss >> autoStr && autoStr == "AUTO") autoRenew = true;
            system.fixedDeposit(id, amount, months, accountPassword, autoRenew);
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
        else if (cmd == "HELP") {
            std::cout << "===== 银行管理系统 命令列表 =====" << std::endl;
            std::cout << "开户 OPEN <id> <S|C> <名称> <金额> [还款日] <密码> <0|1>" << std::endl;
            std::cout << "销户 CLOSE <id> <密码>" << std::endl;
            std::cout << "查询 QUERY <id> <密码>" << std::endl;
            std::cout << "全部查询 QUERYALL" << std::endl;
            std::cout << "存款 DEPOSIT <id> <金额> <密码>" << std::endl;
            std::cout << "取款 WITHDRAW <id> <金额> <密码>" << std::endl;
            std::cout << "转账 TRANSFER <源id> <目标id> <金额> <密码>" << std::endl;
            std::cout << "定期存款 FIXED_DEPOSIT <id> <金额> <期限> <密码> [AUTO]" << std::endl;
            std::cout << "定期支取 FIXED_WITHDRAW <id> <金额> <密码>" << std::endl;
            std::cout << "信用消费 CONSUME <id> <金额> <密码>" << std::endl;
            std::cout << "日期显示 SHOW_DATE" << std::endl;
            std::cout << "日期推进 ADD_DAY <天数>" << std::endl;
            std::cout << "日期设置 SET_DATE <年> <月> <日>" << std::endl;
            std::cout << "创建用户 CREATE_USER <用户名> <密码>" << std::endl;
            std::cout << "删除用户 DELETE_USER <用户名>" << std::endl;
            std::cout << "查询用户 QUERY_USER <用户名>" << std::endl;
            std::cout << "用户列表 QUERY_USERLIST" << std::endl;
            std::cout << "切换用户 SWITCH <用户名> <密码>" << std::endl;
            std::cout << "当前用户 WHOAMI" << std::endl;
            std::cout << "修改账户名 MODIFY NAME <id> <新名> <密码>" << std::endl;
            std::cout << "修改信用额 MODIFY CREDIT <id> <新额度> <密码>" << std::endl;
            std::cout << "修改共享 MODIFY SHARED <id> <ys|ns>" << std::endl;
            std::cout << "添加共有人 ADD_OWNER <id> <用户名> <FULL|RESTRICTED> [限额]" << std::endl;
            std::cout << "移除共有人 REMOVE_OWNER <id> <用户名>" << std::endl;
            std::cout << "修改限额 MODIFY_OWNER_LIMIT <id> <用户名> <新限额>" << std::endl;
            std::cout << "冻结账户 FREEZE <id>" << std::endl;
            std::cout << "解冻账户 UNFREEZE <id>" << std::endl;
            std::cout << "修改用户密码 MODIFY_USERPASSWORD <旧密码> <新密码>" << std::endl;
            std::cout << "修改账户密码 MODIFY_ACCOUNTPASSWORD <id> <旧密码> <新密码>" << std::endl;
            std::cout << "操作记录 LOG" << std::endl;
            std::cout << "回滚 ROLLBACK <日志ID>" << std::endl;
            std::cout << "保存 SAVE <文件名>" << std::endl;
            std::cout << "恢复 RESUME <文件名>" << std::endl;
        }
        else return false;

        return true;
    }
};
