#include "ui.h"
#include <iostream>
#include <sstream>
#include <iomanip>

BankUI::BankUI(BankSystem& sys) : system(sys), currentMode(Mode::NONE) {}

std::string BankUI::getPasswordInput(const std::string &prompt) const {
    std::cout << prompt;
    std::string pwd;
    std::getline(std::cin, pwd);
    return pwd;
}

std::string BankUI::promptLine(const std::string &label) const {
    std::cout << label;
    std::string line;
    if (!std::getline(std::cin, line)) return "";
    if (line == "b" || line == "B") return "";
    return line;
}

bool BankUI::promptInt(const std::string &label, int &out) const {
    while (true) {
        std::cout << label;
        std::string line;
        if (!std::getline(std::cin, line)) return false;
        if (line == "b" || line == "B") return false;
        std::istringstream iss(line);
        int val;
        if (iss >> val) { out = val; return true; }
        std::cout << " [输入无效，请输入整数，输入 b 返回]" << std::endl;
    }
}

bool BankUI::promptDouble(const std::string &label, double &out) const {
    while (true) {
        std::cout << label;
        std::string line;
        if (!std::getline(std::cin, line)) return false;
        if (line == "b" || line == "B") return false;
        std::istringstream iss(line);
        double val;
        if (iss >> val) { out = val; return true; }
        std::cout << " [输入无效，请输入数字，输入 b 返回]" << std::endl;
    }
}

void BankUI::executeAndFeedback(const std::string &commandStr) {
    system.setSilent(true);
    bool parseOk = Command::execute(system, commandStr, m_lastAction);
    system.setSilent(false);
    if (!parseOk) {
        std::cout << " [命令格式错误]" << std::endl;
    } else {
        feedback(m_lastAction, system.lastResult());
    }
}

void BankUI::feedback(const std::string &action, bool ok) const {
    std::string upper = action;
    for (auto i = 0; i < upper.size(); i++) upper[i] = toupper(upper[i]);
    if (!ok) {
        if (upper == "OPEN") std::cout << " [开户失败: 参数错误或ID重复]";
        else if (upper == "CLOSE") std::cout << " [销户失败]";
        else if (upper == "DEPOSIT") std::cout << " [存款失败]";
        else if (upper == "WITHDRAW") std::cout << " [取款失败]";
        else if (upper == "TRANSFER") std::cout << " [转账失败]";
        else if (upper == "QUERY") std::cout << " [查询失败: 权限不足或密码错误]";
        else if (upper == "MODIFY") std::cout << " [修改失败]";
        else if (upper == "MODIFY_SHARED") std::cout << " [修改失败]";
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
        else if (upper == "MODIFY_SHARED") std::cout << " [修改共享状态成功]";
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

// ========== Collector 函数 ==========

std::string BankUI::collectOpen() {
    int id; std::string typeStr, name; double balance; int pwd, sharedInt; int repDay = 0;
    if (!promptInt("请输入账户ID: ", id)) return "";
    while (true) {
        std::string t = promptLine("请输入账户类型 (S=储蓄, C=信用): ");
        if (t.empty()) return "";
        if (t == "S" || t == "C") { typeStr = t; break; }
        std::cout << " [无效类型]" << std::endl;
    }
    name = promptLine("请输入账户名称: ");
    if (name.empty()) return "";
    if (!promptDouble("请输入初始余额: ", balance)) return "";
    if (typeStr == "C") {
        while (true) {
            if (!promptInt("请输入还款日 (1-28): ", repDay)) return "";
            if (repDay >= 1 && repDay <= 28) break;
            std::cout << " [还款日须为1-28]" << std::endl;
        }
    }
    while (true) {
        if (!promptInt("请输入账户密码(6位数字): ", pwd)) return "";
        if (pwd >= 100000 && pwd <= 999999) break;
        std::cout << " [密码必须为6位数字(100000-999999)]" << std::endl;
    }
    while (true) {
        std::string s = promptLine("是否共享账户? (1=共享, 0=独占): ");
        if (s.empty()) return "";
        if (s == "0" || s == "1") { sharedInt = s[0] - '0'; break; }
        std::cout << " [请输入0或1]" << std::endl;
    }
    std::ostringstream oss;
    oss << "OPEN " << id << " " << typeStr << " " << name << " " << balance;
    if (typeStr == "C") oss << " " << repDay;
    oss << " " << pwd << " " << sharedInt;
    return oss.str();
}

std::string BankUI::collectClose() {
    int id, pwd;
    if (!promptInt("请输入账户ID: ", id)) return "";
    if (!promptInt("请输入账户密码: ", pwd)) return "";
    std::ostringstream oss;
    oss << "CLOSE " << id << " " << pwd;
    return oss.str();
}

std::string BankUI::collectQuery() {
    int id, pwd;
    if (!promptInt("请输入账户ID: ", id)) return "";
    if (!promptInt("请输入账户密码: ", pwd)) return "";
    std::ostringstream oss;
    oss << "QUERY " << id << " " << pwd;
    return oss.str();
}

std::string BankUI::collectDeposit() {
    int id; double amount; int pwd;
    if (!promptInt("请输入账户ID: ", id)) return "";
    if (!promptDouble("请输入存款金额: ", amount)) return "";
    if (!promptInt("请输入账户密码: ", pwd)) return "";
    std::ostringstream oss;
    oss << "DEPOSIT " << id << " " << amount << " " << pwd;
    return oss.str();
}

std::string BankUI::collectWithdraw() {
    int id; double amount; int pwd;
    if (!promptInt("请输入账户ID: ", id)) return "";
    if (!promptDouble("请输入取款金额: ", amount)) return "";
    if (!promptInt("请输入账户密码: ", pwd)) return "";
    std::ostringstream oss;
    oss << "WITHDRAW " << id << " " << amount << " " << pwd;
    return oss.str();
}

std::string BankUI::collectTransfer() {
    int srcId, dstId; double amount; int pwd;
    if (!promptInt("请输入转出账户ID: ", srcId)) return "";
    if (!promptInt("请输入转入账户ID: ", dstId)) return "";
    if (!promptDouble("请输入转账金额: ", amount)) return "";
    if (!promptInt("请输入账户密码: ", pwd)) return "";
    std::ostringstream oss;
    oss << "TRANSFER " << srcId << " " << dstId << " " << amount << " " << pwd;
    return oss.str();
}

std::string BankUI::collectFixedDeposit() {
    int id; double amount; int months, pwd;
    if (!promptInt("请输入账户ID: ", id)) return "";
    if (!promptDouble("请输入存款金额: ", amount)) return "";
    if (!promptInt("请输入存款期限(月): ", months)) return "";
    if (!promptInt("请输入账户密码: ", pwd)) return "";
    std::ostringstream oss;
    oss << "FIXED_DEPOSIT " << id << " " << amount << " " << months << " " << pwd;
    return oss.str();
}

std::string BankUI::collectFixedWithdraw() {
    int id; double amount; int pwd;
    if (!promptInt("请输入账户ID: ", id)) return "";
    if (!promptDouble("请输入支取金额: ", amount)) return "";
    if (!promptInt("请输入账户密码: ", pwd)) return "";
    std::ostringstream oss;
    oss << "FIXED_WITHDRAW " << id << " " << amount << " " << pwd;
    return oss.str();
}

std::string BankUI::collectConsume() {
    int id; double amount; int pwd;
    if (!promptInt("请输入账户ID: ", id)) return "";
    if (!promptDouble("请输入消费金额: ", amount)) return "";
    if (!promptInt("请输入账户密码: ", pwd)) return "";
    std::ostringstream oss;
    oss << "CONSUME " << id << " " << amount << " " << pwd;
    return oss.str();
}

std::string BankUI::collectCashAdvance() {
    int id; double amount; int pwd;
    if (!promptInt("请输入账户ID: ", id)) return "";
    if (!promptDouble("请输入取现金额: ", amount)) return "";
    if (!promptInt("请输入账户密码: ", pwd)) return "";
    std::ostringstream oss;
    oss << "CASH_ADVANCE " << id << " " << amount << " " << pwd;
    return oss.str();
}

std::string BankUI::collectModifyName() {
    int id; std::string newName; int pwd;
    if (!promptInt("请输入账户ID: ", id)) return "";
    newName = promptLine("请输入新账户名称: ");
    if (newName.empty()) return "";
    if (!promptInt("请输入账户密码: ", pwd)) return "";
    std::ostringstream oss;
    oss << "MODIFY NAME " << id << " " << newName << " " << pwd;
    return oss.str();
}

std::string BankUI::collectModifyCredit() {
    int id; double credit; int pwd;
    if (!promptInt("请输入账户ID: ", id)) return "";
    if (!promptDouble("请输入新信用额度: ", credit)) return "";
    if (!promptInt("请输入账户密码: ", pwd)) return "";
    std::ostringstream oss;
    oss << "MODIFY CREDIT " << id << " " << credit << " " << pwd;
    return oss.str();
}

std::string BankUI::collectModifyShared() {
    int id; std::string sharedStr;
    if (!promptInt("请输入账户ID: ", id)) return "";
    while (true) {
        sharedStr = promptLine("是否共享? (ys=共享, ns=独占): ");
        if (sharedStr.empty()) return "";
        if (sharedStr == "ys" || sharedStr == "ns") break;
        std::cout << " [请输入ys或ns]" << std::endl;
    }
    std::ostringstream oss;
    oss << "MODIFY SHARED " << id << " " << sharedStr;
    return oss.str();
}

std::string BankUI::collectModifyAccountPassword() {
    int id, oldPwd, newPwd;
    if (!promptInt("请输入账户ID: ", id)) return "";
    while (true) {
        if (!promptInt("请输入旧密码: ", oldPwd)) return "";
        if (oldPwd >= 100000 && oldPwd <= 999999) break;
        std::cout << " [密码必须为6位数字]" << std::endl;
    }
    while (true) {
        if (!promptInt("请输入新密码: ", newPwd)) return "";
        if (newPwd >= 100000 && newPwd <= 999999) break;
        std::cout << " [密码必须为6位数字]" << std::endl;
    }
    std::ostringstream oss;
    oss << "MODIFY_ACCOUNTPASSWORD " << id << " " << oldPwd << " " << newPwd;
    return oss.str();
}

std::string BankUI::collectAddOwner() {
    int id; std::string username;
    if (!promptInt("请输入账户ID: ", id)) return "";
    username = promptLine("请输入要添加的用户名: ");
    if (username.empty()) return "";
    std::ostringstream oss;
    oss << "ADD_OWNER " << id << " " << username;
    return oss.str();
}

std::string BankUI::collectRemoveOwner() {
    int id; std::string username;
    if (!promptInt("请输入账户ID: ", id)) return "";
    username = promptLine("请输入要移除的用户名: ");
    if (username.empty()) return "";
    std::ostringstream oss;
    oss << "REMOVE_OWNER " << id << " " << username;
    return oss.str();
}

std::string BankUI::collectCreateUser() {
    std::string username, password;
    username = promptLine("请输入用户名: ");
    if (username.empty()) return "";
    password = promptLine("请输入密码: ");
    if (password.empty()) return "";
    std::ostringstream oss;
    oss << "CREATE_USER " << username << " " << password;
    return oss.str();
}

std::string BankUI::collectDeleteUser() {
    std::string username = promptLine("请输入要删除的用户名: ");
    if (username.empty()) return "";
    std::ostringstream oss;
    oss << "DELETE_USER " << username;
    return oss.str();
}

std::string BankUI::collectQueryUser() {
    std::string username = promptLine("请输入要查询的用户名: ");
    if (username.empty()) return "";
    std::ostringstream oss;
    oss << "QUERY_USER " << username;
    return oss.str();
}

std::string BankUI::collectSwitch() {
    std::string username, password;
    username = promptLine("请输入用户名: ");
    if (username.empty()) return "";
    password = promptLine("请输入密码: ");
    if (password.empty()) return "";
    std::ostringstream oss;
    oss << "SWITCH " << username << " " << password;
    return oss.str();
}

std::string BankUI::collectModifyUserPassword() {
    std::string oldPwd, newPwd;
    oldPwd = promptLine("请输入旧密码: ");
    if (oldPwd.empty()) return "";
    newPwd = promptLine("请输入新密码: ");
    if (newPwd.empty()) return "";
    std::ostringstream oss;
    oss << "MODIFY_USERPASSWORD " << oldPwd << " " << newPwd;
    return oss.str();
}

std::string BankUI::collectAddDay() {
    int days;
    if (!promptInt("请输入推进天数: ", days)) return "";
    std::ostringstream oss;
    oss << "ADD_DAY " << days;
    return oss.str();
}

std::string BankUI::collectSetDate() {
    int y, m, d;
    if (!promptInt("请输入年份: ", y)) return "";
    if (!promptInt("请输入月份: ", m)) return "";
    if (!promptInt("请输入日期: ", d)) return "";
    std::ostringstream oss;
    oss << "SET_DATE " << y << " " << m << " " << d;
    return oss.str();
}

std::string BankUI::collectRollback() {
    int n;
    if (!promptInt("请输入回滚操作数: ", n)) return "";
    std::ostringstream oss;
    oss << "ROLLBACK " << n;
    return oss.str();
}

std::string BankUI::collectSave() {
    std::string filename = promptLine("请输入文件名: ");
    if (filename.empty()) return "";
    std::ostringstream oss;
    oss << "SAVE " << filename;
    return oss.str();
}

std::string BankUI::collectResume() {
    std::string filename = promptLine("请输入文件名: ");
    if (filename.empty()) return "";
    std::ostringstream oss;
    oss << "RESUME " << filename;
    return oss.str();
}

// ========== Handle 子菜单函数 ==========

void BankUI::handleAccount() {
    while (true) {
        std::cout << "\n--- 账户管理 ---\n";
        std::cout << " [1] 开户\n [2] 销户\n [3] 查询账户\n [4] 查询所有账户\n [0] 返回\n";
        std::string choice = promptLine("请选择: ");
        if (choice == "0") return;
        std::string cmdStr;
        if (choice == "1") cmdStr = collectOpen();
        else if (choice == "2") cmdStr = collectClose();
        else if (choice == "3") cmdStr = collectQuery();
        else if (choice == "4") { executeAndFeedback("QUERYALL"); continue; }
        else { std::cout << " [无效选择]" << std::endl; continue; }
        if (!cmdStr.empty()) executeAndFeedback(cmdStr);
    }
}

void BankUI::handleDepositWithdraw() {
    while (true) {
        std::cout << "\n--- 存取转账 ---\n";
        std::cout << " [1] 存款\n [2] 取款\n [3] 转账\n [0] 返回\n";
        std::string choice = promptLine("请选择: ");
        if (choice == "0") return;
        std::string cmdStr;
        if (choice == "1") cmdStr = collectDeposit();
        else if (choice == "2") cmdStr = collectWithdraw();
        else if (choice == "3") cmdStr = collectTransfer();
        else { std::cout << " [无效选择]" << std::endl; continue; }
        if (!cmdStr.empty()) executeAndFeedback(cmdStr);
    }
}

void BankUI::handleFixedDeposit() {
    while (true) {
        std::cout << "\n--- 定期存款 ---\n";
        std::cout << " [1] 定期存款\n [2] 定期部分提前支取\n [0] 返回\n";
        std::string choice = promptLine("请选择: ");
        if (choice == "0") return;
        std::string cmdStr;
        if (choice == "1") cmdStr = collectFixedDeposit();
        else if (choice == "2") cmdStr = collectFixedWithdraw();
        else { std::cout << " [无效选择]" << std::endl; continue; }
        if (!cmdStr.empty()) executeAndFeedback(cmdStr);
    }
}

void BankUI::handleCredit() {
    while (true) {
        std::cout << "\n--- 信用账户 ---\n";
        std::cout << " [1] 消费\n [2] 取现\n [0] 返回\n";
        std::string choice = promptLine("请选择: ");
        if (choice == "0") return;
        std::string cmdStr;
        if (choice == "1") cmdStr = collectConsume();
        else if (choice == "2") cmdStr = collectCashAdvance();
        else { std::cout << " [无效选择]" << std::endl; continue; }
        if (!cmdStr.empty()) executeAndFeedback(cmdStr);
    }
}

void BankUI::handleModify() {
    while (true) {
        std::cout << "\n--- 账户信息修改 ---\n";
        if (currentMode == Mode::ADMIN) {
            std::cout << " [1] 修改账户名\n [2] 修改信用额度\n [3] 修改共享状态\n [4] 修改账户密码\n [0] 返回\n";
        } else {
            std::cout << " [1] 修改账户名\n [2] 修改账户密码\n [0] 返回\n";
        }
        std::string choice = promptLine("请选择: ");
        if (choice == "0") return;
        std::string cmdStr;
        if (choice == "1") cmdStr = collectModifyName();
        else if (choice == "2") {
            if (currentMode == Mode::ADMIN) cmdStr = collectModifyCredit();
            else cmdStr = collectModifyAccountPassword();
        }
        else if (choice == "3") {
            if (currentMode == Mode::ADMIN) cmdStr = collectModifyShared();
            else { std::cout << " [无效选择]" << std::endl; continue; }
        }
        else if (choice == "4") {
            if (currentMode == Mode::ADMIN) cmdStr = collectModifyAccountPassword();
            else { std::cout << " [无效选择]" << std::endl; continue; }
        }
        else { std::cout << " [无效选择]" << std::endl; continue; }
        if (!cmdStr.empty()) executeAndFeedback(cmdStr);
    }
}

void BankUI::handleShared() {
    while (true) {
        std::cout << "\n--- 共享账户管理 ---\n";
        std::cout << " [1] 添加共有人\n [2] 移除共有人\n [0] 返回\n";
        std::string choice = promptLine("请选择: ");
        if (choice == "0") return;
        std::string cmdStr;
        if (choice == "1") cmdStr = collectAddOwner();
        else if (choice == "2") cmdStr = collectRemoveOwner();
        else { std::cout << " [无效选择]" << std::endl; continue; }
        if (!cmdStr.empty()) executeAndFeedback(cmdStr);
    }
}

void BankUI::handleUserMgmt() {
    while (true) {
        std::cout << "\n--- 用户管理 ---\n";
        std::cout << " [1] 创建用户\n [2] 删除用户\n [3] 查询用户\n [4] 查询所有用户\n [5] 切换用户\n [6] 修改用户密码\n [0] 返回\n";
        std::string choice = promptLine("请选择: ");
        if (choice == "0") return;
        std::string cmdStr;
        if (choice == "1") cmdStr = collectCreateUser();
        else if (choice == "2") cmdStr = collectDeleteUser();
        else if (choice == "3") cmdStr = collectQueryUser();
        else if (choice == "4") { executeAndFeedback("QUERY_USERLIST"); continue; }
        else if (choice == "5") cmdStr = collectSwitch();
        else if (choice == "6") cmdStr = collectModifyUserPassword();
        else { std::cout << " [无效选择]" << std::endl; continue; }
        if (!cmdStr.empty()) executeAndFeedback(cmdStr);
    }
}

void BankUI::handleDateLog() {
    while (true) {
        std::cout << "\n--- 日期与日志 ---\n";
        std::cout << " [1] 显示当前日期\n [2] 推进日期\n [3] 设置日期\n [4] 操作日志\n [5] 回滚\n [6] 保存日志\n [7] 恢复日志\n [0] 返回\n";
        std::string choice = promptLine("请选择: ");
        if (choice == "0") return;
        std::string cmdStr;
        if (choice == "1") { executeAndFeedback("SHOW_DATE"); continue; }
        else if (choice == "2") cmdStr = collectAddDay();
        else if (choice == "3") cmdStr = collectSetDate();
        else if (choice == "4") { executeAndFeedback("LOG"); continue; }
        else if (choice == "5") cmdStr = collectRollback();
        else if (choice == "6") cmdStr = collectSave();
        else if (choice == "7") cmdStr = collectResume();
        else { std::cout << " [无效选择]" << std::endl; continue; }
        if (!cmdStr.empty()) executeAndFeedback(cmdStr);
    }
}

// ========== 模式循环 ==========

void BankUI::adminModeLoop() {
    while (true) {
        std::cout << "\n===== 管理员模式 =====\n";
        std::cout << " [1] 账户管理\n [2] 存取转账\n [3] 定期存款\n [4] 信用账户\n";
        std::cout << " [5] 账户信息修改\n [6] 共享账户管理\n [7] 用户管理\n";
        std::cout << " [8] 日期与日志\n [9] 查询当前用户\n [0] 返回主菜单\n";
        std::string choice = promptLine("请选择: ");
        if (choice == "0") return;
        if (choice == "1") handleAccount();
        else if (choice == "2") handleDepositWithdraw();
        else if (choice == "3") handleFixedDeposit();
        else if (choice == "4") handleCredit();
        else if (choice == "5") handleModify();
        else if (choice == "6") handleShared();
        else if (choice == "7") handleUserMgmt();
        else if (choice == "8") handleDateLog();
        else if (choice == "9") executeAndFeedback("WHOAMI");
        else std::cout << " [无效选择]" << std::endl;
    }
}

void BankUI::userModeLoop() {
    while (true) {
        std::cout << "\n===== 普通用户模式 =====\n";
        std::cout << " [1] 账户管理\n [2] 存取转账\n [3] 定期存款\n [4] 信用账户\n";
        std::cout << " [5] 账户信息修改\n [6] 个人设置\n [7] 查询当前用户\n [0] 返回主菜单\n";
        std::string choice = promptLine("请选择: ");
        if (choice == "0") return;
        if (choice == "1") handleAccount();
        else if (choice == "2") handleDepositWithdraw();
        else if (choice == "3") handleFixedDeposit();
        else if (choice == "4") handleCredit();
        else if (choice == "5") handleModify();
        else if (choice == "6") {
            while (true) {
                std::cout << "\n--- 个人设置 ---\n";
                std::cout << " [1] 切换用户\n [2] 修改用户密码\n [0] 返回\n";
                std::string c = promptLine("请选择: ");
                if (c == "0") break;
                std::string cmdStr;
                if (c == "1") cmdStr = collectSwitch();
                else if (c == "2") cmdStr = collectModifyUserPassword();
                else { std::cout << " [无效选择]" << std::endl; continue; }
                if (!cmdStr.empty()) executeAndFeedback(cmdStr);
            }
        }
        else if (choice == "7") executeAndFeedback("WHOAMI");
        else std::cout << " [无效选择]" << std::endl;
    }
}

// ========== 主入口 ==========

void BankUI::run() {
    while (true) {
        std::cout << "\n============================\n";
        std::cout << "    银行管理系统\n";
        std::cout << "============================\n";
        std::cout << " [1] 管理员登录\n [2] 普通用户登录\n [0] 退出系统\n";
        std::cout << "============================\n";
        std::string choice = promptLine("请选择: ");
        if (choice.empty()) continue;

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
                currentMode = Mode::ADMIN;
                adminModeLoop();
                currentMode = Mode::NONE;
            } else {
                std::cout << "密码错误\n";
            }
        } else if (choice == "2") {
            std::string username = promptLine("请输入用户名: ");
            if (username.empty()) continue;
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
                currentMode = Mode::USER;
                userModeLoop();
                currentMode = Mode::NONE;
            } else {
                std::cout << "用户名或密码错误\n";
            }
        } else {
            std::cout << "无效选择\n";
        }
    }
}
