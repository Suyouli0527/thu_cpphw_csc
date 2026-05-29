#pragma once
#include <string>
#include <vector>
#include <algorithm>

enum class UserType {
    admin,
    normal
};

class User {
private:
    std::string userName;
    std::string userPassword;
    std::vector<int> accountIDs;
    UserType userType;

public:
    User(const std::string &name, UserType type, const std::string &pwd)
        : userName(name), userPassword(pwd), userType(type) {}

    std::string getUserName() const { return userName; }
    std::vector<int> getAccountIDs() const { return accountIDs; }
    int getAccountCount() const { return accountIDs.size(); }
    UserType getUserType() const { return userType; }

    void addAccountID(int id) { accountIDs.push_back(id); }
    void removeAccountID(int id) {
        accountIDs.erase(std::remove(accountIDs.begin(), accountIDs.end(), id), accountIDs.end());
    }
    bool isAdmin() const { return userType == UserType::admin; }
    bool verifyPassword(const std::string &pwd) const { return userPassword == pwd; }
    bool changePassword(const std::string &oldPwd, const std::string &newPwd) {
        if (userPassword != oldPwd || newPwd.empty()) return false;
        userPassword = newPwd;
        return true;
    }
    bool operator<(const User &other) const { return userName < other.userName; }
};
