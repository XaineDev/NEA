//
// Created by Xaine on 28/03/2023.
//

#include <iostream>
#include "external/json/json.hpp"

class LibraryBook {
protected:
    int id;
    std::string title;
    std::string author;

    nlohmann::json bookJson;
public:
    explicit LibraryBook(nlohmann::json bookJson);

    [[nodiscard]] int getId() const;

    [[nodiscard]] std::string getTitle() const;

    [[nodiscard]] std::string getAuthor() const;

    bool hasOwner(int owner) const;
    int getOwner() const;

    virtual nlohmann::json claimBook(const std::string &username, const std::string &token) = 0;
};

class OwnedLibraryBook : public LibraryBook {
private:
    int owner;
public:
    explicit OwnedLibraryBook(nlohmann::json bookJson);
    ~OwnedLibraryBook();

    [[nodiscard]] int getOwner() const;

    nlohmann::json claimBook(const std::string &username, const std::string &token) override;
};

class UnownedLibraryBook : public LibraryBook {
public:
    explicit UnownedLibraryBook(nlohmann::json bookJson);
    ~UnownedLibraryBook();

    nlohmann::json claimBook(const std::string &username, const std::string &token) override;
};

class LibraryAccount {
private:
    int id;
    std::string username;
    long long creationDate;
    std::string creationDateString;
    std::string token;
    bool admin;
    // might remove this
    // before profile picture is loaded we will use the default profile picture
    // after it is loaded we will cache the profile picture in the users appdata directory
    bool fetchedProfilePicture = false;

    unsigned long long totalBooks = 0;
    LibraryBook **books;
public:
    explicit LibraryAccount(nlohmann::json accountJson);

    ~LibraryAccount();

    [[nodiscard]] std::string getUsername() const;

    [[nodiscard]] int getId() const;

    [[nodiscard]] long long getCreationDate() const;

    [[nodiscard]] bool hasFetchedProfilePicture() const;

    [[nodiscard]] std::string getToken() const;

    void setFetchedProfilePicture(bool fetched);

    [[nodiscard]] bool isAdmin() const;

    LibraryBook **getCheckedOutBooks();

    int getAmountOfBooks() const;

    std::string getCreationDateString();

    LibraryBook *getBook(int i);

    void removeBook(LibraryBook *pBook);

    void updateBooks(LibraryBook **pBook, int i);

};