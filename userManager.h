#pragma once
#include "user.h"
#include <vector>
#include <string>

class UserManager {
public:
    UserManager() = delete;

    static User* findUser(const std::vector<User>& users, const std::string& name);
    static bool isAdmin(const std::vector<User>& users, const std::string& currentUserName);
    static bool isLoggedIn(const std::string& currentUserName);
    static bool isLegalName(const std::string& name);
    static bool ownsAccount(const std::vector<User>& users, const std::string& currentUserName, int id);

    static bool createUser(std::vector<User>& users, const std::string& currentUserName, const std::string& username);
    static bool deleteUser(std::vector<User>& users, const std::string& currentUserName, const std::string& username);
    static bool switchUser(const std::vector<User>& users, std::string& currentUserName, const std::string& username);

    static bool queryAllUser(const std::vector<User>& users, const std::string& currentUserName);
    static void whoami(const std::string& currentUserName);
    static void addAccountToUser(std::vector<User>& users, const std::string& username, int accountId);
    static void removeAccountFromUser(std::vector<User>& users, const std::string& username, int accountId);
    static void reset(std::vector<User>& users, std::string& currentUserName);
};
