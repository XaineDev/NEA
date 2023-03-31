//
// Created by Xaine on 28/03/2023.
//

#include "LibraryAccount.h"

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

LibraryAccount::LibraryAccount(std::string username, long long int creationDate, std::string token) : username(std::move(username)){
    this->creationDate = creationDate;
    this->token = std::move(token);
}
