//
// Created by Xaine on 04/04/2023.
//

#include "json.hpp"

class LibraryBook {
private:
    int id;
    std::string title;
    std::string author;
    std::string currentOwner;

    nlohmann::json bookJson;
public:
    explicit LibraryBook(nlohmann::json bookJson);

    [[nodiscard]] int getId() const;

    [[nodiscard]] std::string getTitle() const;

    [[nodiscard]] std::string getAuthor() const;

    [[nodiscard]] std::string getCurrentOwner() const;
};
