//
// Created by Xaine on 30/03/2023.
//


#include <string>
#include "../external/json/json.hpp"

class WebRequest {
private:
    static const std::string apiurl;

    static nlohmann::json request(const std::string &endpoint, const std::string &method, const nlohmann::json &body);

public:
    static nlohmann::json login(const std::string &username, const std::string &password);

    static nlohmann::json registerAccount(const std::string &username, const std::string &password);

    static nlohmann::json claimBook(const nlohmann::json& requestJson);

    static nlohmann::json getBooks(const nlohmann::json& requestJson);

    static nlohmann::json returnBook(const nlohmann::json& json);

    static nlohmann::json fetchLogs(const nlohmann::json& json);

    static nlohmann::json forceCheckout(const nlohmann::json& json);

    static nlohmann::json fetchUsers(const nlohmann::json& json);
};
