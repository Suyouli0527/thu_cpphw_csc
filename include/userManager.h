#pragma once
#include "user.h"
#include "accountManager.h"
#include <vector>
#include <string>

class UserManager {
private:
    std::vector<User> users;
    std::string currentUserName;
    AccountManager* accountMgr;

public:
    UserManager(AccountManager* am);

    User* findUser(const std::string &name) const;
    bool isAdmin() const;
    bool isLoggedIn() const;
    static bool isLegalName(const std::string &name);
    bool ownsAccount(int id) const;

    bool createUser(const std::string &username, const std::string &password);
    bool deleteUser(const std::string &username);
    bool switchUser(const std::string &username, const std::string &password);
    bool queryAllUser() const;
    void whoami() const;

    void addAccountToUser(const std::string &username, int accountId);
    void removeAccountFromUser(const std::string &username, int accountId);

    void reset();
    const std::string& getCurrentUserName() const { return currentUserName; }
};
