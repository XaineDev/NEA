//
// Created by Xaine on 28/03/2023.
//

#include "LibraryAccount.h"
#include "external/time/date/date.h"
#include "net/WebRequest.h"

#include <utility>

std::string LibraryAccount::getUsername() const {
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
    // check to make sure the account json is valid
    this->username = accountJson["username"].get<std::string>();
    this->creationDate = accountJson["created_at"].get<long long>();
    this->token = accountJson["token"].get<std::string>();
    this->admin = accountJson["isAdmin"].get<bool>();
    this->id = accountJson["id"].get<int>();

    // convert creation date to a string
    auto creation = date::year_month_day{
            date::floor<date::days>(std::chrono::system_clock::from_time_t(this->creationDate))};
    this->creationDateString = std::string(date::format("%d/%m/%Y", creation));

    if (accountJson["currentBooks"].is_null() || accountJson["currentBooks"].empty()) {
        this->totalBooks = 0;
        return;
    }

    totalBooks = accountJson["currentBooks"].size();
    this->books = new LibraryBook *[totalBooks];
    for (int i = 0; i < totalBooks; i++) {
        if (accountJson["currentBooks"][i]["currentOwner"].is_null() || accountJson["currentBooks"][i]["currentOwner"].get<int>() == 0) {
            this->books[i] = new UnownedLibraryBook(accountJson["currentBooks"][i]);
        } else {
            this->books[i] = new OwnedLibraryBook(accountJson["currentBooks"][i]);
        }
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
    return this->creationDateString;
}

LibraryBook *LibraryAccount::getBook(int i) {
    return this->books[i];
}

void LibraryAccount::removeBook(LibraryBook *pBook) {
    for (int i = 0; i < this->totalBooks; i++) {
        if (this->books[i] == pBook) {
            delete this->books[i];
            this->books[i] = nullptr;
            this->totalBooks--;
            return;
        }
    }
}

void LibraryAccount::updateBooks(LibraryBook **pBook, int bookCount) {
    // clear books on account
    for (int i = 0; i < this->totalBooks; i++) {
        delete this->books[i];
    }

    // filter out all books that are not owned by the user
    int newBookAmount = 0;
    auto newBooks = new LibraryBook*[bookCount];
    for (int i = 0; i < bookCount; i++) {
        int bookOwner = pBook[i]->getOwner();
        if (bookOwner == this->id) {
            newBooks[newBookAmount] = pBook[i];
            newBookAmount++;
        }
    }

    if (newBookAmount == 0) {
        this->books = new LibraryBook*[1];
        this->totalBooks = 0;
        delete[] newBooks;
        return;
    }

    // update books
    delete[] this->books;
    this->books = new LibraryBook*[newBookAmount];
    for (int i = 0; i < newBookAmount; i++) {
        this->books[i] = newBooks[i];
    }
    this->totalBooks = newBookAmount;

    // free memory used
    delete[] newBooks;
}

LibraryBook::LibraryBook(nlohmann::json bookJson) {
    this->id = bookJson["id"].get<int>();
    this->title = bookJson["title"].get<std::string>();
    this->author = bookJson["author"].get<std::string>();
    this->bookJson = bookJson;
}

int LibraryAccount::getId() const {
    return this->id;
}

std::string LibraryBook::getTitle() const {
    return this->title;
}

std::string LibraryBook::getAuthor() const {
    return this->author;
}

bool LibraryBook::hasOwner(int id) const {
    if (!this->bookJson.contains("currentOwner")) {
        return false;
    }
    if (this->bookJson["currentOwner"].is_null()) {
        return false;
    }
    return this->bookJson["currentOwner"].get<int>() == id;
}

int LibraryBook::getOwner() const {
    std::cout << this->bookJson.dump() << std::endl;
    if (this->bookJson.contains("currentOwner")) {
        return this->bookJson["currentOwner"].get<int>();
    }
    return 0;
}

int LibraryBook::getId() const {
    return this->id;
}

OwnedLibraryBook::OwnedLibraryBook(nlohmann::json bookJson) : LibraryBook(bookJson) {
    this->owner = bookJson["currentOwner"].get<int>();
}

OwnedLibraryBook::~OwnedLibraryBook() = default;

int OwnedLibraryBook::getOwner() const {
    return this->owner;
}

nlohmann::json OwnedLibraryBook::claimBook(const std::string &username, const std::string &token) {
    nlohmann::json json;
    json["success"] = false;
    json["message"] = "This book is already owned.";
    return json;
}

UnownedLibraryBook::UnownedLibraryBook(nlohmann::json bookJson) : LibraryBook(bookJson) {

}

UnownedLibraryBook::~UnownedLibraryBook() = default;

nlohmann::json UnownedLibraryBook::claimBook(const std::string &username, const std::string &token) {
    nlohmann::json requestJson;
    requestJson["book_id"] = this->getId();
    requestJson["book_title"] = this->getTitle(); // used to check if the id is correct for the book

    requestJson["user"]["username"] = username;
    requestJson["user"]["token"] = token;

    return WebRequest::claimBook(requestJson);
}