//
// Created by Xaine on 04/04/2023.
//

#include "LibraryBook.h"

LibraryBook::LibraryBook(nlohmann::json bookJson) {
    this->id = bookJson["id"].get<int>();
    this->title = bookJson["title"].get<std::string>();
    this->author = bookJson["author"].get<std::string>();
    this->currentOwner = bookJson["currentOwner"].get<std::string>();
}

int LibraryBook::getId() const {
    return this->id;
}

std::string LibraryBook::getTitle() const {
    return this->title;
}

std::string LibraryBook::getAuthor() const {
    return this->author;
}

std::string LibraryBook::getCurrentOwner() const {
    return this->currentOwner;
}
