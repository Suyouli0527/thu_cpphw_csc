#include "userManager.h"
#include <algorithm>
#include <iostream>

User* UserManager::findUser(const std::vector<User>& users, const std::string& name) {
    for (auto it = users.begin(); it != users.end(); ++it) {
        if (it->getUserName() == name) {
            return const_cast<User*>(&(*it));
        }
    }
    return nullptr;
}

bool UserManager::isAdmin(const std::vector<User>& users, const std::string& currentUserName) {
    User* user = findUser(users, currentUserName);
    return user != nullptr && user->isAdmin();
}

bool UserManager::isLoggedIn(const std::string& currentUserName) {
    return !currentUserName.empty();
}

bool UserManager::isLegalName(const std::string& name) {
    if (name.empty()) return false;
    for (char c : name) {
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {
            return false;
        }
    }
    return true;
}

bool UserManager::ownsAccount(const std::vector<User>& users, const std::string& currentUserName, int id) {
    User* user = findUser(users, currentUserName);
    if (!user) return false;
    const std::vector<int>& ids = user->getAccountIDs();
    for (int aid : ids) {
        if (aid == id) return true;
    }
    return false;
}

bool UserManager::createUser(std::vector<User>& users, const std::string& currentUserName, const std::string& username) {
    if (!isAdmin(users, currentUserName)) return false;
    if (!isLegalName(username)) return false;
    if (username == "admin") return false;
    if (findUser(users, username)) return false;
    users.emplace_back(username, UserType::normal);
    return true;
}

bool UserManager::deleteUser(std::vector<User>& users, const std::string& currentUserName, const std::string& username) {
    if (!isAdmin(users, currentUserName)) return false;
    if (username == "admin") return false;
    if (username == currentUserName) return false;
    User* user = findUser(users, username);
    if (!user) return false;
    if (user->getAccountCount() > 0) return false;
    for (size_t i = 0; i < users.size(); i++) {
        if (users[i].getUserName() == username) {
            users.erase(users.begin() + static_cast<std::ptrdiff_t>(i));
            return true;
        }
    }
    return false;
}

bool UserManager::switchUser(const std::vector<User>& users, std::string& currentUserName, const std::string& username) {
    User* user = findUser(users, username);
    if (!user) return false;
    currentUserName = username;
    return true;
}

bool UserManager::queryAllUser(const std::vector<User>& users, const std::string& currentUserName) {
    if (!isAdmin(users, currentUserName)) return false;
    if (users.empty()) return false;
    std::vector<User> sortedUsers = users;
    std::sort(sortedUsers.begin(), sortedUsers.end());
    for (const auto& user : sortedUsers) {
        std::cout << "USER " << user.getUserName() << " " << user.getAccountCount() << std::endl;
    }
    return true;
}

void UserManager::whoami(const std::string& currentUserName) {
    if (currentUserName.empty()) return;
    std::cout << currentUserName << std::endl;
}

void UserManager::addAccountToUser(std::vector<User>& users, const std::string& username, int accountId) {
    User* user = findUser(users, username);
    if (user) user->addAccountID(accountId);
}

void UserManager::removeAccountFromUser(std::vector<User>& users, const std::string& username, int accountId) {
    User* user = findUser(users, username);
    if (user) user->removeAccountID(accountId);
}

void UserManager::reset(std::vector<User>& users, std::string& currentUserName) {
    users.clear();
    users.emplace_back("admin", UserType::admin);
    users.emplace_back("default", UserType::normal);
    currentUserName = "default";
}
