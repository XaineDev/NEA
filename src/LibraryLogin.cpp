//
// Created by Xaine on 28/03/2023.
//

#include <thread>
#include "LibraryLogin.h"
#include "external/json/json.hpp"
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
        if (apiResponse.contains("success") && apiResponse["success"].get<bool>()) {
            apiAccount = apiResponse["user"];
            account = new LibraryAccount(apiAccount);
            loggedIn = true;
            loginFailed = false;
        } else {
            loggedIn = false;
            loginFailed = true;
            loginFailedTime = time(nullptr);
            loginFailedMessage = apiResponse.contains("error") ? apiResponse["error"].get<std::string>() : std::string(
                    "Couldn't connect to server.");
        }
        loggingIn = false;
    });
    loginThread.detach();
}

LibraryAccount *LibraryLogin::getAccount() {
    if (isLoggedIn()) {
        return account;
    }
    return nullptr;
}

void LibraryLogin::logOut() {
    username[0] = '\0';
    password[0] = '\0';
    loggedIn = false;
    loginFailed = false;
    loginFailedTime = 0;
    loginFailedMessage = "";
    delete account;
    apiResponse = nlohmann::json();
    apiAccount = nlohmann::json();
    logoutQueued = false;
}

void LibraryLogin::registerAccount() {

    if (username[0] == '\0' || password[0] == '\0') return;

    registering = true;
    // create thread to log in asynchronously
    std::thread registerThread([this]() {
        // simulate login
        std::this_thread::sleep_for(std::chrono::seconds(2));

        apiResponse = WebRequest::registerAccount(username, password);
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

std::string LibraryLogin::getLoginFailedMessage() {
    return loginFailedMessage;
}

LibraryLogin::~LibraryLogin() {
    delete account;
}

nlohmann::json LibraryLogin::getApiResponse() {
    return apiResponse;
}

void LibraryLogin::queueLogout() {
    this->logoutQueued = true;
}

bool LibraryLogin::isLogoutQueued() {
    return this->logoutQueued;
}