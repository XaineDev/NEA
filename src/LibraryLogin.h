//
// Created by Xaine on 28/03/2023.
//

#include "LibraryAccount.h"
#include "json.hpp"

class LibraryLogin {
private:
    bool loggedIn = false;
    bool loggingIn = false;
    bool loginFailed = false;
    long long loginFailedTime = 0;

    bool registering = false;
    bool registerFailed = false;
    long long registerFailedTime = 0;

public:
    bool isLoggedIn() const;

    bool isLoggingIn() const;
    bool isLoginFailed() const;
    void setLoginFailed(bool failed);
    long long timeSinceLoginFailed() const;

    bool isRegistering() const;
    bool isRegisterFailed() const;
    void setRegisterFailed(bool failed);
    long long timeSinceRegisterFailed() const;

    void login();
    LibraryAccount* getAccount();

    std::string token;
    char username[128] = { 0 };
    char password[128] = { 0 };

    void logOut();
    void registerAccount();

    nlohmann::json apiResponse;
};

