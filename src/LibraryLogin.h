//
// Created by Xaine on 28/03/2023.
//

class LibraryLogin {
private:
    bool loggedIn = false;
    bool loggingIn = false;
    bool loginFailed = false;
    long long loginFailedTime = 0;
public:
    bool isLoggedIn() const;
    bool isLoggingIn() const;
    bool isLoginFailed() const;
    void setLoginFailed(bool failed);
    long long timeSinceLoginFailed() const;
    bool login();

    char username[128] = { 0 };
    char password[128] = { 0 };
};

