//
// Created by Xaine on 30/03/2023.
//


#include <string>
#include "../json.hpp"

class WebRequest {
private:
    static const std::string apiurl;
public:
    static nlohmann::json login(const std::string& username, const std::string& password);
    static nlohmann::json registerAccount(const std::string& username, const std::string& password);
};
