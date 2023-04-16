//
// Created by Xaine on 30/03/2023.
//

#include "WebRequest.h"
#include "../external/http/HTTPRequest.hpp"
#include "../external/json/json.hpp"

#include <iostream>

const std::string WebRequest::apiurl = "http://localhost:3000/api";

nlohmann::json WebRequest::login(const std::string &username, const std::string &password) {
    nlohmann::json bodyJson;
    bodyJson["username"] = username;
    bodyJson["password"] = password;

    return request("login", "POST", bodyJson);
}

nlohmann::json WebRequest::registerAccount(const std::string &username, const std::string &password) {
    nlohmann::json bodyJson;
    bodyJson["username"] = username;
    bodyJson["password"] = password;

    return request("register", "POST", bodyJson);
}

nlohmann::json WebRequest::claimBook(const nlohmann::json& requestJson) {
    return request("claim", "POST", requestJson);
}

nlohmann::json WebRequest::getBooks(const nlohmann::json& requestJson) {
    return request("getBooks", "POST", requestJson);
}

nlohmann::json WebRequest::request(const std::string &endpoint, const std::string &method, const nlohmann::json &body) {
    try {
        http::Request request{apiurl + "/" + endpoint};
        const auto response = request.send(method, body.dump(), {
                {"Content-Type", "application/json"}
        }, (std::chrono::milliseconds) 5000);
        nlohmann::json responseJson = nlohmann::json::parse(std::string{response.body.begin(), response.body.end()});
        return responseJson;
    }
    catch (const std::exception &e) {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }

    return {};
}

nlohmann::json WebRequest::returnBook(const nlohmann::json& json) {
    return request("unclaimBook", "POST", json);
}