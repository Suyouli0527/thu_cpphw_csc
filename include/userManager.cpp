#include "userManager.h"
#include <algorithm>
#include <iostream>

UserManager::UserManager(AccountManager* am)
    : currentUserName("default"), accountMgr(am) {
    users.emplace_back("admin", UserType::admin, "admin");
    users.emplace_back("default", UserType::normal, "default");
}

User* UserManager::findUser(const std::string &name) const {
    for (const auto &user : users) {
        if (user.getUserName() == name) {
            return const_cast<User*>(&user);
        }
    }
    return nullptr;
}

bool UserManager::isAdmin() const {
    User* user = findUser(currentUserName);
    return user != nullptr && user->isAdmin();
}

bool UserManager::isLoggedIn() const {
    return !currentUserName.empty();
}

bool UserManager::isLegalName(const std::string &name) {
    if (name.empty()) return false;
    for (char c : name) {
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {
            return false;
        }
    }
    return true;
}

bool UserManager::ownsAccount(int id) const {
    if (!accountMgr) return false;
    Account* acc = accountMgr->findAccount(id);
    if (!acc) return false;
    for (const auto &o : acc->getOwners()) {
        if (o.name == currentUserName) return true;
    }
    return false;
}

bool UserManager::createUser(const std::string &username, const std::string &password) {
    if (!isAdmin()) return false;
    if (!isLegalName(username)) return false;
    if (username == "admin") return false;
    if (password.empty()) return false;
    if (findUser(username)) return false;
    users.emplace_back(username, UserType::normal, password);
    return true;
}

bool UserManager::deleteUser(const std::string &username) {
    if (!isAdmin()) return false;
    if (username == "admin") return false;
    if (username == currentUserName) return false;
    User* user = findUser(username);
    if (!user) return false;
    if (user->getAccountCount() > 0) return false;
    for (auto i = 0; i < (int)users.size(); i++) {
        if (users[i].getUserName() == username) {
            users.erase(users.begin() + i);
            return true;
        }
    }
    return false;
}

bool UserManager::switchUser(const std::string &username, const std::string &password) {
    User* user = findUser(username);
    if (!user) return false;
    if (!user->verifyPassword(password)) return false;
    currentUserName = username;
    return true;
}

bool UserManager::queryAllUser() const {
    if (!isAdmin()) return false;
    if (users.empty()) return false;
    std::vector<User> sortedUsers = users;
    std::sort(sortedUsers.begin(), sortedUsers.end());
    for (const auto &user : sortedUsers) {
        std::cout << "USER " << user.getUserName() << " " << user.getAccountCount() << std::endl;
    }
    return true;
}

void UserManager::whoami() const {
    if (currentUserName.empty()) return;
    std::cout << currentUserName << std::endl;
}

void UserManager::addAccountToUser(const std::string &username, int accountId) {
    User* user = findUser(username);
    if (user) user->addAccountID(accountId);
}

void UserManager::removeAccountFromUser(const std::string &username, int accountId) {
    User* user = findUser(username);
    if (user) user->removeAccountID(accountId);
}

void UserManager::reset() {
    users.clear();
    users.emplace_back("admin", UserType::admin, "admin");
    users.emplace_back("default", UserType::normal, "default");
    currentUserName = "default";
}
