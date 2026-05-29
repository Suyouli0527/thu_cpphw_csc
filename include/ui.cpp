#include "ui.h"
#include <iostream>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

BankUI::BankUI(BankSystem& sys) : system(sys), cmd(sys), currentMode(Mode::NONE), insertedCardId(-1) {}

std::string BankUI::getPasswordInput(const std::string &prompt) const {
    std::cout << prompt;
    std::string pwd;
#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r' && ch != '\n') {
        if (ch == '\b') {
            if (!pwd.empty()) pwd.pop_back();
        } else {
            pwd += ch;
            std::cout << '*';
        }
    }
    std::cout << std::endl;
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    std::getline(std::cin, pwd);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    return pwd;
}

void BankUI::showHelp(Mode mode) const {
    std::cout << "\n--- 可用命令 ---\n";
    if (mode == Mode::ADMIN) {
        std::cout << "  CREATE_USER username password   创建用户\n";
        std::cout << "  DELETE_USER username           删除用户\n";
        std::cout << "  QUERY_USER username             查询用户\n";
        std::cout << "  QUERY_USERLIST                  查询所有用户\n";
        std::cout << "  ADD_OWNER id username            添加共有人\n";
        std::cout << "  REMOVE_OWNER id username         移除共有人\n";
    }
    std::cout << "  OPEN id type name balance pwd shared(0/1) [repDay]  开立账户\n";
    std::cout << "  CLOSE id pwd                     关闭账户\n";
    std::cout << "  DEPOSIT id amount pwd              存款\n";
    std::cout << "  WITHDRAW id amount pwd             取款\n";
    std::cout << "  QUERY id pwd                      查询账户\n";
    std::cout << "  QUERYALL                          查询所有账户\n";
    std::cout << "  MODIFY NAME id newname pwd        修改账户名\n";
    std::cout << "  MODIFY CREDIT id credit pwd        修改信用额度\n";
    std::cout << "  TRANSFER src dst amount pwd        转账\n";
    std::cout << "  FIXED_DEPOSIT id amount months pwd 定期存款\n";
    std::cout << "  FIXED_WITHDRAW id amount pwd       定期提前支取\n";
    std::cout << "  CONSUME id amount pwd              消费(信用)\n";
    std::cout << "  CASH_ADVANCE id amount pwd         取现(信用)\n";
    std::cout << "  MODIFY_USERPASSWORD old new        修改用户密码\n";
    std::cout << "  MODIFY_ACCOUNTPASSWORD id old new   修改账户密码\n";
    std::cout << "  ADD_DAY days                       推进日期\n";
    std::cout << "  SET_DATE y m d                     设置日期\n";
    std::cout << "  SHOW_DATE                         显示当前日期\n";
    std::cout << "  WHOAMI                             当前用户\n";
    std::cout << "  LOG                               操作日志\n";
    std::cout << "  ROLLBACK n                         回滚\n";
    std::cout << "  HELP                              显示帮助\n";
    if (mode == Mode::CARD) {
        std::cout << "  EJECT_CARD                         退出插卡模式\n";
    } else {
        std::cout << "  EXIT / LOGOUT                     退出登录\n";
    }
    std::cout << "-----------------\n\n";
}

void BankUI::showMainMenu() {
    std::cout << "\n============================\n";
    std::cout << "    银行管理系统\n";
    std::cout << "============================\n";
    std::cout << " [1] 管理员登录\n";
    std::cout << " [2] 普通用户登录\n";
    std::cout << " [3] 插卡操作\n";
    std::cout << " [0] 退出系统\n";
    std::cout << "============================\n";
}

void BankUI::feedback(const std::string &action, bool ok) const {
    std::string upper = action;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    if (!ok) {
        if (upper == "OPEN") std::cout << " [开户失败: 参数错误或ID重复]";
        else if (upper == "CLOSE") std::cout << " [销户失败]";
        else if (upper == "DEPOSIT") std::cout << " [存款失败]";
        else if (upper == "WITHDRAW") std::cout << " [取款失败]";
        else if (upper == "TRANSFER") std::cout << " [转账失败]";
        else if (upper == "QUERY") std::cout << " [查询失败: 权限不足或密码错误]";
        else if (upper == "MODIFY") std::cout << " [修改失败]";
        else if (upper == "MODIFY_USERPASSWORD") std::cout << " [修改失败: 旧密码错误]";
        else if (upper == "MODIFY_ACCOUNTPASSWORD") std::cout << " [修改失败]";
        else if (upper == "ADD_OWNER") std::cout << " [添加共有人失败]";
        else if (upper == "REMOVE_OWNER") std::cout << " [移除共有人失败]";
        else if (upper == "CREATE_USER") std::cout << " [创建用户失败]";
        else if (upper == "DELETE_USER") std::cout << " [删除用户失败]";
        else std::cout << " [操作失败]";
    } else {
        if (upper == "OPEN") std::cout << " [开户成功]";
        else if (upper == "CLOSE") std::cout << " [销户成功]";
        else if (upper == "DEPOSIT") std::cout << " [存款成功]";
        else if (upper == "WITHDRAW") std::cout << " [取款成功]";
        else if (upper == "TRANSFER") std::cout << " [转账成功]";
        else if (upper == "FIXED_DEPOSIT") std::cout << " [定期存款成功]";
        else if (upper == "FIXED_WITHDRAW") std::cout << " [定期部分提前支取成功]";
        else if (upper == "CONSUME") std::cout << " [消费成功]";
        else if (upper == "CASH_ADVANCE") std::cout << " [取现成功]";
        else if (upper == "MODIFY") std::cout << " [修改成功]";
        else if (upper == "MODIFY_USERPASSWORD") std::cout << " [用户密码修改成功]";
        else if (upper == "MODIFY_ACCOUNTPASSWORD") std::cout << " [账户密码修改成功]";
        else if (upper == "ADD_OWNER") std::cout << " [添加共有人成功]";
        else if (upper == "REMOVE_OWNER") std::cout << " [移除共有人成功]";
        else if (upper == "CREATE_USER") std::cout << " [创建用户成功]";
        else if (upper == "DELETE_USER") std::cout << " [删除用户成功]";
        else if (upper == "ADD_DAY" || upper == "SET_DATE") std::cout << " [日期更新成功]";
        else std::cout << " [操作成功]";
    }
    std::cout << std::endl;
}

static const std::set<std::string> cardAllowed = {
    "DEPOSIT", "WITHDRAW", "QUERY", "CLOSE", "MODIFY",
    "TRANSFER", "FIXED_DEPOSIT", "FIXED_WITHDRAW",
    "CONSUME", "CASH_ADVANCE"
};

bool BankUI::executeCommand(const std::string &input) {
    std::istringstream iss(input);
    std::string action;
    iss >> action;

    if (action == "HELP") { showHelp(currentMode); return true; }
    if (action == "EXIT" || action == "LOGOUT" || action == "EJECT_CARD") return false;

    std::string commandToExecute;

    if (currentMode == Mode::CARD && insertedCardId != -1) {
        if (cardAllowed.find(action) == cardAllowed.end()) {
            std::cout << " [插卡模式下不支持此命令]" << std::endl;
            return true;
        }
        std::ostringstream rewritten;
        rewritten << action << " " << insertedCardId;
        std::string rest;
        std::getline(iss, rest);
        if (!rest.empty()) rewritten << rest;
        commandToExecute = rewritten.str();
    } else {
        commandToExecute = input;
    }

    system.setSilent(true);
    bool parseOk = cmd.execute(commandToExecute);
    system.setSilent(false);

    if (!parseOk) {
        std::cout << " [命令格式错误]" << std::endl;
    } else {
        feedback(cmd.lastAction(), system.lastResult());
    }
    return true;
}

void BankUI::adminLoop() {
    std::cout << "\n===== 管理员模式 =====\n";
    std::string line;
    while (true) {
        std::cout << "\nadmin[管理员] > ";
        if (!std::getline(std::cin, line) || line.empty()) continue;
        if (!executeCommand(line)) break;
    }
}

void BankUI::userLoop() {
    std::cout << "\n===== 普通用户模式 =====\n";
    std::string line;
    while (true) {
        std::cout << "\n" << system.getCurrentUserName() << "[普通用户] > ";
        if (!std::getline(std::cin, line) || line.empty()) continue;
        if (!executeCommand(line)) break;
    }
}

void BankUI::cardLoop() {
    std::cout << "\n===== 插卡模式 (账户 #" << insertedCardId << ") =====\n";
    std::cout << "  提示: 账户命令无需输入 id，已自动锁定到当前账户\n";
    showHelp(Mode::CARD);
    std::string line;
    while (true) {
        std::cout << "\nCARD[" << insertedCardId << "] > ";
        if (!std::getline(std::cin, line) || line.empty()) continue;
        if (!executeCommand(line)) break;
    }
    insertedCardId = -1;
}

void BankUI::run() {
    while (true) {
        showMainMenu();
        std::cout << "请选择: ";
        std::string choice;
        if (!std::getline(std::cin, choice)) break;

        if (choice == "0") {
            std::cout << "感谢使用，再见！\n";
            break;
        } else if (choice == "1") {
            std::string pwd = getPasswordInput("请输入管理员密码: ");
            system.setSilent(true);
            system.switchUser("admin", pwd);
            system.setSilent(false);
            std::ostringstream buf;
            std::streambuf *old = std::cout.rdbuf(buf.rdbuf());
            system.whoami();
            std::cout.rdbuf(old);
            if (buf.str().find("admin") != std::string::npos) {
                std::cout << "\n===== 管理员登录成功 =====\n";
                adminLoop();
            } else {
                std::cout << "密码错误\n";
            }
        } else if (choice == "2") {
            std::cout << "请输入用户名: ";
            std::string username;
            if (!std::getline(std::cin, username)) break;
            std::string pwd = getPasswordInput("请输入密码: ");
            system.setSilent(true);
            system.switchUser(username, pwd);
            system.setSilent(false);
            std::ostringstream buf;
            std::streambuf *old = std::cout.rdbuf(buf.rdbuf());
            system.whoami();
            std::cout.rdbuf(old);
            if (buf.str().find(username) != std::string::npos) {
                std::cout << "\n===== 登录成功 =====\n";
                userLoop();
            } else {
                std::cout << "用户名或密码错误\n";
            }
        } else if (choice == "3") {
            std::cout << "请输入账户ID: ";
            std::string idStr;
            if (!std::getline(std::cin, idStr)) break;
            int id = std::stoi(idStr);
            std::string pwd = getPasswordInput("请输入账户密码: ");
            Account* acc = system.findAccountForTest(id);
            if (acc && acc->verifyAccountPassword(std::stoi(pwd))) {
                insertedCardId = id;
                if (!acc->getOwners().empty()) {
                    system.setSilent(true);
                    system.switchUser(acc->getOwners()[0], "dummy");
                    system.setSilent(false);
                }
                cardLoop();
            } else {
                std::cout << "账户不存在或密码错误\n";
            }
        } else {
            std::cout << "无效选择\n";
        }
    }
}
