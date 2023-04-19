//
// Created by Xaine on 28/03/2023.
//

#include <iostream>
#include "LibraryLogin.h"
#include "Image.h"
#include <windows.h>
#include <vector>

class LibrarySystem {
private:
    LibraryLogin *libraryLogin;
    HWND window;
    Image defaultProfileImage = Image(nullptr, 0, 0);

    char searchBarText[128] = {0};
    std::string currentSearch;
    bool hasCachedSearch = false;
    std::vector<LibraryBook*> cachedSearchBooks;
    enum DisplayScreen {
        MENU,
        LIBRARY,
        SEARCH,
        SETTINGS,
        ADMIN_PANEL
    };
    DisplayScreen currentScreen = MENU;
    enum AdminScreen {
        ADMIN_USERS,
        ADMIN_LOGS
    };
    AdminScreen currentAdminScreen = ADMIN_LOGS;

    int logUserTarget = 0;
    int logAmount = 25;
    int logPage = 0;
    bool fetchLogs = true;
    nlohmann::json logResponse;

    int userAmount = 25;
    int userPage = 0;
    bool fetchUsers = true;
    nlohmann::json userResponse;

    bool fetchingBooks = false;
    bool fetchedBooks = false;
    bool failedToFetchBooks = false;
    int libraryBookCount = 0;
    LibraryBook **libraryBooks;

public:
    void drawLibraryScreen();

    explicit LibrarySystem(HWND window, Image defaultProfileImage);

    ~LibrarySystem();

    void drawSecondaryMenuBar();

    void drawMenuScreen();

    void drawLibrary();

    void drawSearch();

    void drawSettings();

    void drawAdminPanel();

    void drawAdminUsers();

    void drawAdminLogs();

    void fetchBooks();

    LibraryLogin* getLoginSystem();

    void tryFetchLogs();

    void tryFetchUsers();
};
