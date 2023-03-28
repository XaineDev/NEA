//
// Created by Xaine on 28/03/2023.
//

#include <iostream>
#include "LibraryLogin.h"
#include <windows.h>

class LibrarySystem {
private:
    LibraryLogin libraryLogin;
    HWND window;
public:
    void drawLibraryScreen();
    explicit LibrarySystem(HWND window);
};


