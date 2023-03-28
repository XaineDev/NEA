//
// Created by Xaine on 28/03/2023.
//

#include <thread>
#include <string.h>
#include "LibraryLogin.h"

bool LibraryLogin::isLoggedIn() const {
    return loggedIn;
}

bool LibraryLogin::isLoggingIn() const {
    return loggingIn;
}

bool LibraryLogin::isLoginFailed() const {
    return loginFailed;
}

void LibraryLogin::setLoginFailed(bool failed) {
    loginFailed = failed;
}

long long LibraryLogin::timeSinceLoginFailed() const {
    return time(nullptr) - loginFailedTime;
}

bool LibraryLogin::login() {
    loggingIn = true;
    // create thread to log in asynchronously
    std::thread loginThread([this]() {
        // simulate login
        std::this_thread::sleep_for(std::chrono::seconds(2));

        if (strcmp(username, "admin") == 0 && strcmp(password, "password") == 0)
        {
            loggedIn = true;
        } else {
            loginFailed = true;
            loginFailedTime = time(nullptr);
        }

        loggingIn = false;
    });
    loginThread.detach();
    return isLoggedIn();
}
