#pragma once
#include "Tools.h"
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
        std::vector<int> accountIDs;
        UserType userType;
    public:
        User(const std::string &name, UserType type) : userName(name), userType(type) {}
        std::string getUserName() const { return userName; }
        std::vector<int> getAccountIDs() const { return accountIDs; }
        int getAccountCount() const { return accountIDs.size(); }
        UserType getUserType() const { return userType; }
        void addAccountID(const int &id) { accountIDs.push_back(id); }
        void removeAccountID(const int &id) {
            accountIDs.erase(std::remove(accountIDs.begin(), accountIDs.end(), id), accountIDs.end());
        }

        bool isAdmin() const { return userType == UserType::admin; }
        bool operator<(const User& other) const {
        return userName < other.userName;
        }
    };
