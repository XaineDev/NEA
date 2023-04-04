//
// Created by Xaine on 28/03/2023.
//

#include "LibraryAccount.h"
#include "external/time/date/date.h"

#include <utility>

std::string LibraryAccount::getUsername() const {
    std::cout << "getting username" << std::endl;
    return username;
}

long long LibraryAccount::getCreationDate() const {
    return creationDate;
}

bool LibraryAccount::hasFetchedProfilePicture() const {
    return fetchedProfilePicture;
}

void LibraryAccount::setFetchedProfilePicture(bool fetched) {
    fetchedProfilePicture = fetched;
}

std::string LibraryAccount::getToken() const {
    return token;
}

bool LibraryAccount::isAdmin() const {
    return admin;
}

LibraryAccount::LibraryAccount(nlohmann::json accountJson) {
    // print out formatted account json
    std::cout << accountJson.dump(4) << std::endl;

    // check to make sure the account json is valid
    this->username = accountJson["username"].get<std::string>();
    this->creationDate = accountJson["created_at"].get<long long>();
    this->token = accountJson["token"].get<std::string>();
    this->admin = accountJson["isAdmin"].get<bool>();

    // convert creation date to a string
    auto creation = date::year_month_day{
            date::floor<date::days>(std::chrono::system_clock::from_time_t(this->creationDate))};
    this->creationDateString = std::string(date::format("%d/%m/%Y", creation));
    std::cout << "creation date string: " << this->creationDateString << std::endl;

    if (accountJson["currentBooks"].is_null()) {
        this->totalBooks = 0;
        return;
    }

    totalBooks = accountJson["currentBooks"].size();
    this->books = new LibraryBook *[totalBooks];
    for (int i = 0; i < totalBooks; i++) {
        this->books[i] = new LibraryBook(accountJson["currentBooks"][i]);
    }
}

LibraryAccount::~LibraryAccount() {
    if (this->totalBooks != 0) {
        for (int i = 0; i < totalBooks; i++) {
            delete this->books[i];
        }
    }

}

int LibraryAccount::getAmountOfBooks() const {
    return (int) this->totalBooks;
}

LibraryBook **LibraryAccount::getCheckedOutBooks() {
    return this->books;
}

std::string LibraryAccount::getCreationDateString() {
    std::cout << "getting creation date string" << std::endl;
    return this->creationDateString;
}
