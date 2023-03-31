//
// Created by Xaine on 28/03/2023.
//

#include <iostream>

class LibraryAccount {
private:
    const std::string username;
    long long creationDate;
    std::string token;
    bool admin;
    // before profile picture is loaded we will use the default profile picture
    // after it is loaded we will cache the profile picture in the users appdata directory
    bool fetchedProfilePicture = false;
public:
    LibraryAccount(std::string username, long long creationDate, std::string token, bool admin);

    std::string getUsername() const;

    long long getCreationDate() const;

    bool hasFetchedProfilePicture() const;

    std::string getToken() const;

    void setFetchedProfilePicture(bool fetched);

    bool isAdmin() const;
};

