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
    LibraryAccount *account;
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

public:
    void drawLibraryScreen();

    explicit LibrarySystem(HWND window, Image defaultProfileImage);

    void drawSecondaryMenuBar();

    void drawMenuScreen();

    void drawLibrary();

    void drawSearch();

    void drawSettings();
};
