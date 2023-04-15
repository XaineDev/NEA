//
// Created by Xaine on 28/03/2023.
//

#include "LibraryAccount.h"
#include "external/json/json.hpp"

class LibraryLogin {
private:
    bool loggedIn = false;
    bool loggingIn = false;
    bool loginFailed = false;
    long long loginFailedTime = 0;
    std::string loginFailedMessage;

    bool registering = false;
    bool registerFailed = false;
    long long registerFailedTime = 0;

    nlohmann::json apiResponse;
    nlohmann::json apiAccount;

    LibraryAccount *account = nullptr;

public:
    ~LibraryLogin();

    [[nodiscard]] bool isLoggedIn() const;

    [[nodiscard]] bool isLoggingIn() const;

    [[nodiscard]] bool isLoginFailed() const;

    void setLoginFailed(bool failed);

    [[nodiscard]] long long timeSinceLoginFailed() const;

    [[nodiscard]] bool isRegistering() const;

    [[nodiscard]] bool isRegisterFailed() const;

    void setRegisterFailed(bool failed);

    [[nodiscard]] long long timeSinceRegisterFailed() const;

    void login();

    LibraryAccount *getAccount();

    std::string token;
    char username[128] = {0};
    char password[128] = {0};

    void logOut();

    void registerAccount();

    std::string getLoginFailedMessage();

    nlohmann::json getApiResponse();
};

