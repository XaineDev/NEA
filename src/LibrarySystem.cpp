//
// Created by Xaine on 28/03/2023.
//

#include "LibrarySystem.h"
#include "external/imgui.h"
#include "net/WebRequest.h"

LibrarySystem::LibrarySystem(HWND window, Image defaultProfileImage) {
    this->libraryLogin = new LibraryLogin();
    this->window = window;
    this->defaultProfileImage = defaultProfileImage;
}

LibrarySystem::~LibrarySystem() {
    delete libraryLogin;
}

void LibrarySystem::drawLibraryScreen() {
    if (!libraryLogin->isLoggedIn()) {
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 1, 1));
        if (ImGui::Begin("Login", nullptr,
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove)) {
            auto windowSize = ImVec2(350, 150);
            ImGui::SetWindowSize(windowSize);
            RECT rect;
            GetWindowRect(window, &rect);
            auto windowPos = ImVec2(((float) rect.right - (float) rect.left) / 2 - windowSize.x / 2,
                                    ((float) rect.bottom - (float) rect.top) / 2 - windowSize.y / 2);
            ImGui::SetWindowPos(windowPos);

            if (libraryLogin->isLoggingIn()) {
                ImGui::Text("Logging in...");
            } else if (libraryLogin->isRegistering()) {
                ImGui::Text("Registering...");
            } else {
                if (libraryLogin->isLoginFailed() && libraryLogin->timeSinceLoginFailed() < 2) {
                    ImGui::Text("Login failed. ");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), libraryLogin->getLoginFailedMessage().c_str());
                } else if (libraryLogin->isRegisterFailed() && libraryLogin->timeSinceRegisterFailed() < 2) {
                    std::string registerError = libraryLogin->getApiResponse()["error"].get<std::string>();
                    ImGui::Text("Registration failed. ");
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), registerError.c_str());
                } else {
                    ImGui::Text("Please login to continue");
                    ImGui::InputText("Username", libraryLogin->username, 128);
                    ImGui::InputText("Password", libraryLogin->password, 128, ImGuiInputTextFlags_Password);
                    if (ImGui::Button("Login")) {
                        libraryLogin->login();
                    }
                    ImGui::SameLine();
                    ImGui::Spacing();
                    ImGui::SameLine();
                    if (ImGui::Button("Register")) {
                        libraryLogin->registerAccount();
                    }
                }
            }

            ImGui::End();
        }
        ImGui::PopStyleColor(1);
        return;
    }

    // check if books have been loaded
    if (!fetchedBooks) {
        fetchBooks();
        fetchedBooks = true;
    }

    bool loggingOut = false;

    // if logged in, show the user the proper main menu bar
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.106f, 0.106f, 0.106f, 1.00f));
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem("Main Menu")) {
            currentScreen = MENU;
        }
        if (ImGui::MenuItem("My Books")) {
            currentScreen = LIBRARY;
        }
        if (ImGui::MenuItem("Search")) {
            currentScreen = SEARCH;
        }
        if (ImGui::MenuItem("Settings")) {
            currentScreen = SETTINGS;
        }
        if (libraryLogin->getAccount()->isAdmin()) {
            if (ImGui::MenuItem("User Lookup")) {
                currentScreen = LOOKUP;
            }
        }

        ImGui::SameLine(
                ImGui::GetWindowWidth() - ImGui::CalcTextSize("Logout ").x - ImGui::GetStyle().ItemSpacing.x * 2);

        if (ImGui::MenuItem("Logout")) {
            loggingOut = true;
            libraryLogin->logOut();
        }
        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleColor(1);

    if (loggingOut) {
        return;
    }

    drawSecondaryMenuBar();


    switch (currentScreen) {
        case MENU:
            drawMenuScreen();
            break;
        case LIBRARY:
            drawLibrary();
            break;
        case SEARCH:
            drawSearch();
            break;
        case SETTINGS:
            drawSettings();
            break;
        case LOOKUP:
            break;
    }
}

void LibrarySystem::drawSecondaryMenuBar() {
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Set position to the bottom of the viewport
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - ImGui::GetFrameHeight()));

        // Extend width to viewport width
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, ImGui::GetFrameHeight()));

        // Add menu bar flag and disable everything else
        ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_MenuBar;

        if (ImGui::Begin("StatusBar", nullptr, flags)) {
            if (ImGui::BeginMenuBar()) {
                ImGui::Text("Logged in as %s", libraryLogin->username);
                ImGui::EndMenuBar();
            }
            ImGui::End();
        }
    }
}

void LibrarySystem::drawMenuScreen() {
    if (ImGui::Begin("Profile")) {
        //ImGui::Image((void *) defaultProfileImage.texture, ImVec2(128, 128));
        //ImGui::SameLine();
        // loop spacing until text is to the right of the image
        //for (int i = 0; i < 5; i++) {
        //    ImGui::Spacing();
        //    ImGui::SameLine();
        //}

        // convert unix time to datepoint

        ImGui::Text("Welcome, %s", libraryLogin->getAccount()->getUsername().c_str());
        ImGui::Text("You have %d books checked out", libraryLogin->getAccount()->getAmountOfBooks());
        ImGui::Text("");
        ImGui::Text("Account Status: %s", libraryLogin->getAccount()->isAdmin() ? "Admin" : "User");
        ImGui::Text("Account Created: %s", libraryLogin->getAccount()->getCreationDateString().c_str());


        ImGui::End();
    }
}

void LibrarySystem::drawLibrary() {

}

void LibrarySystem::drawSearch() {

}

void LibrarySystem::drawSettings() {

}

LibraryLogin* LibrarySystem::getLoginSystem() {
    return libraryLogin;
}

void LibrarySystem::fetchBooks() {
    nlohmann::json request;
    request["user"]["username"] = libraryLogin->getAccount()->getUsername();
    request["user"]["token"] = libraryLogin->getAccount()->getToken();
    nlohmann::json books = WebRequest::getBooks(request);

    std::printf("Fetching books... \n");

    if (!books["error"].is_null() && !books["error"].get<std::string>().empty()) {
        std::printf("Error: %s \n", books["error"].get<std::string>().c_str());
        return;
    }

    if (books["books"].is_null() || books["books"].empty()) {
        std::printf("Error: No books found \n");
        return;
    }

    this->libraryBookCount = (int)books["books"].size();
    this->libraryBooks = new LibraryBook *[libraryBookCount];

    for (int i = 0; i < libraryBookCount; i++) {
        if (books["books"][i]["currentOwner"].is_null() || books["books"][i]["currentOwner"].get<std::string>().empty()) {
            libraryBooks[i] = new UnownedLibraryBook(books["books"][i]);
        } else {
            libraryBooks[i] = new OwnedLibraryBook(books["books"][i]);
        }
    }

    std::printf("Fetched %d book(s)", libraryBookCount);

}
