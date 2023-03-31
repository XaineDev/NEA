//
// Created by Xaine on 30/03/2023.
//

#include "WebRequest.h"
#include "HTTPRequest.hpp"
#include "../json.hpp"

#include <iostream>

const std::string WebRequest::apiurl = "http://localhost:3000/api";

nlohmann::json WebRequest::login(const std::string& username, const std::string& password) {
    std::cout << "Logging in with username: " << username << " and password: " << password << std::endl;

    try
    {
        http::Request request{apiurl + "/login"};
        nlohmann::json bodyJson;
        bodyJson["username"] = username;
        bodyJson["password"] = password;
        const std::string body = bodyJson.dump();
        const auto response = request.send("POST", body, {
                {"Content-Type", "application/json"}
        });
        nlohmann::json responseJson = nlohmann::json::parse(std::string{response.body.begin(), response.body.end()});
        return responseJson;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }
    return {};
}

nlohmann::json WebRequest::registerAccount(const std::string& username, const std::string& password) {
    std::cout << "Registering with username: " << username << " and password: " << password << std::endl;

    try
    {
        http::Request request{apiurl + "/register"};
        nlohmann::json bodyJson;
        bodyJson["username"] = username;
        bodyJson["password"] = password;
        const std::string body = bodyJson.dump();
        const auto response = request.send("POST", body, {
                {"Content-Type", "application/json"}
        });
        nlohmann::json responseJson = nlohmann::json::parse(std::string{response.body.begin(), response.body.end()});
        return responseJson;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }
    nlohmann::json failedJson;
    failedJson["success"] = false;
    return failedJson;
}