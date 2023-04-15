//
// Created by Xaine on 28/03/2023.
//

#include <iostream>
#include "LibraryLogin.h"
#include "Image.h"
#include <windows.h>

class LibrarySystem {
private:
    LibraryLogin *libraryLogin;
    HWND window;
    Image defaultProfileImage = Image(nullptr, 0, 0);

    enum DisplayScreen {
        MENU,
        LIBRARY,
        SEARCH,
        SETTINGS,
        LOOKUP
    };
    DisplayScreen currentScreen = MENU;

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

    void fetchBooks();

    LibraryLogin* getLoginSystem();
};
