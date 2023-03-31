//
// Created by Xaine on 28/03/2023.
//

#include <thread>
#include <string.h>
#include "LibraryLogin.h"
#include "json.hpp"
#include "net/WebRequest.h"

bool LibraryLogin::isLoggedIn() const {
    return loggedIn;
}

bool LibraryLogin::isLoggingIn() const {
    return loggingIn;
}

bool LibraryLogin::isRegistering() const {
    return registering;
}

bool LibraryLogin::isRegisterFailed() const {
    return registerFailed;
}

void LibraryLogin::setRegisterFailed(bool failed) {
    registerFailed = failed;
}

long long LibraryLogin::timeSinceRegisterFailed() const {
    return time(nullptr) - registerFailedTime;
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

void LibraryLogin::login() {

    if (username[0] == '\0' || password[0] == '\0') return;

    loggingIn = true;
    // create thread to log in asynchronously
    std::thread loginThread([this]() {
        // simulate login
        std::this_thread::sleep_for(std::chrono::seconds(2));

        apiResponse = WebRequest::login(username, password);
        if (apiResponse["success"].get<bool>()) {
            loggedIn = true;
            loginFailed = false;
        } else {
            loggedIn = false;
            loginFailed = true;
            loginFailedTime = time(nullptr);
        }
        loggingIn = false;
    });
    loginThread.detach();
}

LibraryAccount* LibraryLogin::getAccount() {
    if (isLoggedIn()) {
        return new LibraryAccount(username, apiResponse["creationDate"].get<long long>(), apiResponse["token"].get<std::string>());
    }
    return nullptr;
}

void LibraryLogin::logOut() {
    loggedIn = false;
    loginFailed = false;
    loginFailedTime = 0;
    memset(username, 0, sizeof(username));
    memset(password, 0, sizeof(password));
}

void LibraryLogin::registerAccount() {

    if (username[0] == '\0' || password[0] == '\0') return;

    registering = true;
    // create thread to log in asynchronously
    std::thread registerThread([this]() {
        // simulate login
        std::this_thread::sleep_for(std::chrono::seconds(2));

        apiResponse = WebRequest::registerAccount(username, password);
        std::cout << apiResponse.dump() << std::endl;
        if (apiResponse["success"].get<bool>()) {
            registerFailed = false;
        } else {
            registerFailed = true;
            registerFailedTime = time(nullptr);
        }
        registering = false;
    });
    registerThread.detach();
}