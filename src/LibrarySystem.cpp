//
// Created by Xaine on 28/03/2023.
//

#include "LibrarySystem.h"
#include "external/imgui.h"
#include "net/WebRequest.h"
#include "external/imgui_internal.h"
#include "search.h"
#include "external/time/date/date.h"

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

    if (libraryLogin->isLogoutQueued()) {
        libraryLogin->logOut();
        currentScreen = MENU;

        // cleanup books for next user
        fetchedBooks = false;
        cachedSearchBooks.clear();
        hasCachedSearch = false;
        searchBarText[0] = '\0';
        currentSearch = "";
        return;
    }

    // check if books have been loaded
    if (!fetchedBooks) {
        fetchBooks();
        fetchedBooks = true;
    }

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
        case ADMIN_PANEL:
            // check if user is admin, if not change screen back to menu
            if (!libraryLogin->getAccount()->isAdmin()) {
                currentScreen = MENU;
                drawMenuScreen();
            }
            drawAdminPanel();
            break;
    }

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.106f, 0.106f, 0.106f, 1.00f));
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem("Main Menu")) {
            currentScreen = MENU;
        }
        if (ImGui::MenuItem("My Books")) {
            fetchedBooks = false;
            currentScreen = LIBRARY;
        }
        if (ImGui::MenuItem("Search")) {
            fetchedBooks = false;
            currentScreen = SEARCH;
        }
        if (ImGui::MenuItem("Settings")) {
            currentScreen = SETTINGS;
        }
        if (libraryLogin->getAccount()->isAdmin()) {
            if (ImGui::MenuItem("Admin Panel")) {
                currentScreen = ADMIN_PANEL;
            }
        }

        ImGui::SameLine(
                ImGui::GetWindowWidth() - ImGui::CalcTextSize("Logout ").x - ImGui::GetStyle().ItemSpacing.x * 2);

        if (ImGui::MenuItem("Logout")) {
            fetchedBooks = false;
            libraryLogin->queueLogout();
        }
        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleColor(1);

    drawSecondaryMenuBar();

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
    float topMargin = 20;

    int window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("books", nullptr, window_flags)) {

        // fullscreen window
        ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - topMargin*2));
        ImGui::SetWindowPos(ImVec2(0, topMargin));

        for (int i = 0; i < libraryLogin->getAccount()->getAmountOfBooks(); i++) {
            LibraryBook* book = libraryLogin->getAccount()->getBook(i);
            std::string bookTitle = book->getTitle() + " " + std::to_string(i);

            if (ImGui::BeginChild(bookTitle.c_str(), ImVec2(480, 60), true)) {
                ImGui::Text("Title: %s", book->getTitle().c_str());
                ImGui::Text("Author: %s", book->getAuthor().c_str());
                ImGui::EndChild();
            }
            ImGui::SameLine();

            if (ImGui::BeginChild((bookTitle + " Buttons").c_str(), ImVec2(65, 60), true)) {
                if (ImGui::Button("Return")) {
                    nlohmann::json request;

                    request["user"]["username"] = libraryLogin->getAccount()->getUsername();
                    request["user"]["token"] = libraryLogin->getAccount()->getToken();
                    request["book_id"] = book->getId();
                    request["book_title"] = book->getTitle();

                    nlohmann::json response = WebRequest::returnBook(request);
                    if ((!response["error"].is_null() && !response["error"].get<std::string>().empty()) || response.empty()) {
                        std::printf("Error returning book: %s \n", response["error"].get<std::string>().c_str());
                        if (response["error"].get<std::string>() == "invalid token") {
                            fetchedBooks = false;
                            libraryLogin->queueLogout();
                        }
                    } else {
                        if (!response["success"].is_null() && response["success"].get<bool>()) {
                            std::printf("Successfully returned book \n");
                            libraryLogin->getAccount()->removeBook(book);
                            fetchedBooks = false;
                        }
                    }
                }
                ImGui::EndChild();
            }
        }

        ImGui::End();
    }
}

void LibrarySystem::drawSearch() {
    float topMargin = 20;
    int window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    if (ImGui::Begin("SearchBar", nullptr, window_flags)) {
        ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - topMargin*2));
        ImGui::SetWindowPos(ImVec2(0, topMargin));

        ImGui::InputText("##SearchBar", this->searchBarText, 128);
        ImGui::SameLine();
        if (ImGui::Button("Search")) {
            cachedSearchBooks.clear();
            hasCachedSearch = false;
            currentSearch = searchBarText;
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear")) {
            cachedSearchBooks.clear();
            hasCachedSearch = false;
            currentSearch = "";
            this->searchBarText[0] = '\0';
        }

        bool updatingBooks = false;

        // create search results box which fills up rest of screen
        if (ImGui::BeginChild("SearchResults", ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - topMargin*3), false)) {
            if (currentSearch.empty()) {
                for (int id = 0; id < this->libraryBookCount; id++) {
                    LibraryBook* book = libraryBooks[id];
                    std::string bookTitle = book->getTitle() + " " + std::to_string(id);

                    ImGui::BeginChild(bookTitle.c_str(), ImVec2(480, 60), true);
                    ImGui::Text("Title: %s", book->getTitle().c_str());
                    ImGui::Text("Author: %s", book->getAuthor().c_str());
                    ImGui::EndChild();
                    ImGui::SameLine();
                    ImGui::BeginChild((bookTitle + " Buttons").c_str(), ImVec2(150, 60), true);
                    if (book->hasOwner(this->getLoginSystem()->getAccount()->getId())) {
                        // show return button as you already own the book
                        if (ImGui::Button("Return")) {
                            updatingBooks = true;
                            nlohmann::json request;

                            request["user"]["username"] = libraryLogin->getAccount()->getUsername();
                            request["user"]["token"] = libraryLogin->getAccount()->getToken();
                            request["book_id"] = book->getId();
                            request["book_title"] = book->getTitle();

                            nlohmann::json response = WebRequest::returnBook(request);
                            if ((!response["error"].is_null() && !response["error"].get<std::string>().empty()) || response.empty()) {
                                std::printf("Error returning book: %s \n", response["error"].get<std::string>().c_str());
                                if (response["error"].get<std::string>() == "invalid token") {
                                    fetchedBooks = false;
                                    libraryLogin->queueLogout();
                                }
                            } else {
                                if (!response["success"].is_null() && response["success"].get<bool>()) {
                                    std::printf("Successfully returned book \n");
                                    libraryLogin->getAccount()->removeBook(book);
                                    fetchedBooks = false;
                                }
                            }
                            cachedSearchBooks.clear();
                            hasCachedSearch = false;
                        }
                    } else if (book->hasOwner(0)) {
                        // show checkout button as the book is not owned by anyone
                        if (ImGui::Button("Checkout")) {
                            updatingBooks = true;
                            nlohmann::json claimBookRequestJson;
                            claimBookRequestJson["user"]["username"] = libraryLogin->getAccount()->getUsername();
                            claimBookRequestJson["user"]["token"] = libraryLogin->getAccount()->getToken();
                            claimBookRequestJson["book_id"] = book->getId();
                            claimBookRequestJson["book_title"] = book->getTitle();
                            auto response = WebRequest::claimBook(claimBookRequestJson);
                            if ((!response["error"].is_null() && !response["error"].get<std::string>().empty()) ||
                                response.empty()) {
                                std::printf("Error claiming book: %s \n",
                                            response["error"].get<std::string>().c_str());
                                if (response["error"].get<std::string>() == "invalid token") {
                                    fetchedBooks = false;
                                    libraryLogin->queueLogout();
                                }
                            } else {
                                if (!response["success"].is_null() && response["success"].get<bool>()) {
                                    std::printf("Successfully claimed book \n");
                                    fetchedBooks = false;
                                }
                            }
                            cachedSearchBooks.clear();
                            hasCachedSearch = false;
                        }
                    } else {
                        // show unavailable button as the book is owned by someone else
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

                        ImGui::Button("Unavailable");

                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();

                        // if user is admin, show force checkout button
                        if (libraryLogin->getAccount()->isAdmin()) {
                            if (ImGui::Button("Force Checkout")) {
                                nlohmann::json request;
                                request["authentication"]["username"] = libraryLogin->getAccount()->getUsername();
                                request["authentication"]["token"] = libraryLogin->getAccount()->getToken();
                                request["authentication"]["id"] = libraryLogin->getAccount()->getId();
                                request["book_id"] = book->getId();
                                request["book_title"] = book->getTitle();
                                request["current_owner"] = book->getOwner();
                                auto response = WebRequest::forceCheckout(request);
                                if ((!response["error"].is_null() && !response["error"].get<std::string>().empty()) ||
                                    response.empty()) {
                                    std::printf("Error force checking out book: %s \n",
                                                response["error"].get<std::string>().c_str());
                                    if (response["error"].get<std::string>() == "unauthorized") {
                                        fetchedBooks = false;
                                        libraryLogin->queueLogout();
                                    }
                                } else {
                                    if (!response["success"].is_null() && response["success"].get<bool>()) {
                                        std::printf("Successfully force checked out book \n");
                                        fetchedBooks = false;
                                    }
                                }
                            }
                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip("Force checkout will remove the book from the current owner");
                            }
                        }

                    }
                    ImGui::EndChild();

                    if (updatingBooks) {
                        break;
                    }

                }
            } else {
                // filter search results
                if (!hasCachedSearch) {
                    std::printf("Searching for: %s \n", currentSearch.c_str());
                    for (int id = 0; id < this->libraryBookCount; id++) {
                        LibraryBook* book = libraryBooks[id];
                        if (Search::findString(book->getTitle(), currentSearch) || Search::findString(book->getAuthor(), currentSearch)) {
                            std::printf("Found book: %s \n", book->getTitle().c_str());
                            cachedSearchBooks.push_back(book);
                        }
                    }
                    hasCachedSearch = true;
                }

                if (cachedSearchBooks.empty() || !fetchedBooks) {
                    ImGui::Text("No results found");
                }

                for (LibraryBook* book : cachedSearchBooks) {
                    std::string bookTitle = book->getTitle() + " " + std::to_string(book->getId());

                    ImGui::BeginChild(bookTitle.c_str(), ImVec2(480, 60), true);
                    ImGui::Text("Title: %s", book->getTitle().c_str());
                    ImGui::Text("Author: %s", book->getAuthor().c_str());
                    ImGui::EndChild();
                    ImGui::SameLine();
                    ImGui::BeginChild((bookTitle + " Buttons").c_str(), ImVec2(150, 60), true);
                    if (book->hasOwner(this->getLoginSystem()->getAccount()->getId())) {
                        // show return button as you already own the book
                        if (ImGui::Button("Return")) {
                            updatingBooks = true;
                            nlohmann::json request;

                            request["user"]["username"] = libraryLogin->getAccount()->getUsername();
                            request["user"]["token"] = libraryLogin->getAccount()->getToken();
                            request["book_id"] = book->getId();
                            request["book_title"] = book->getTitle();

                            nlohmann::json response = WebRequest::returnBook(request);
                            if ((!response["error"].is_null() && !response["error"].get<std::string>().empty()) ||
                                response.empty()) {
                                std::printf("Error returning book: %s \n",
                                            response["error"].get<std::string>().c_str());
                                if (response["error"].get<std::string>() == "invalid token") {
                                    fetchedBooks = false;
                                    libraryLogin->queueLogout();
                                }
                            } else {
                                if (!response["success"].is_null() && response["success"].get<bool>()) {
                                    std::printf("Successfully returned book \n");
                                    fetchedBooks = false;
                                }
                            }
                            cachedSearchBooks.clear();
                            hasCachedSearch = false;
                        }
                    } else if (book->hasOwner(0)) {
                        // show checkout button as the book is not owned by anyone
                        if (ImGui::Button("Checkout")) {
                            updatingBooks = true;
                            nlohmann::json claimBookRequestJson;
                            claimBookRequestJson["user"]["username"] = libraryLogin->getAccount()->getUsername();
                            claimBookRequestJson["user"]["token"] = libraryLogin->getAccount()->getToken();
                            claimBookRequestJson["book_id"] = book->getId();
                            claimBookRequestJson["book_title"] = book->getTitle();
                            auto response = WebRequest::claimBook(claimBookRequestJson);
                            if ((!response["error"].is_null() && !response["error"].get<std::string>().empty()) ||
                                response.empty()) {
                                std::printf("Error claiming book: %s \n",
                                            response["error"].get<std::string>().c_str());
                                if (response["error"].get<std::string>() == "invalid token") {
                                    fetchedBooks = false;
                                    libraryLogin->queueLogout();
                                }
                            } else {
                                if (!response["success"].is_null() && response["success"].get<bool>()) {
                                    std::printf("Successfully claimed book \n");
                                    fetchedBooks = false;
                                }
                            }
                            cachedSearchBooks.clear();
                            hasCachedSearch = false;
                        }
                    } else {
                        // show unavailable button as the book is owned by someone else
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

                        ImGui::Button("Unavailable");

                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();

                        // if user is admin, show force checkout button
                        if (libraryLogin->getAccount()->isAdmin()) {
                            if (ImGui::Button("Force Checkout")) {
                                nlohmann::json request;
                                request["authentication"]["username"] = libraryLogin->getAccount()->getUsername();
                                request["authentication"]["token"] = libraryLogin->getAccount()->getToken();
                                request["authentication"]["id"] = libraryLogin->getAccount()->getId();
                                request["book_id"] = book->getId();
                                request["book_title"] = book->getTitle();
                                request["current_owner"] = book->getOwner();
                                auto response = WebRequest::forceCheckout(request);
                                if ((!response["error"].is_null() && !response["error"].get<std::string>().empty()) ||
                                    response.empty()) {
                                    std::printf("Error force checking out book: %s \n",
                                                response["error"].get<std::string>().c_str());
                                    if (response["error"].get<std::string>() == "unauthorized") {
                                        fetchedBooks = false;
                                        libraryLogin->queueLogout();
                                    }
                                } else {
                                    if (!response["success"].is_null() && response["success"].get<bool>()) {
                                        std::printf("Successfully force checked out book \n");
                                        fetchedBooks = false;
                                    }
                                }
                            }
                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip("Force checkout will remove the book from the current owner");
                            }
                        }
                    }
                    ImGui::EndChild();

                    if (updatingBooks) {
                        break;
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
}

void LibrarySystem::drawSettings() {

}

void LibrarySystem::drawAdminPanel() {
    float topMargin = 20;
    int window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    if (ImGui::Begin("AdminPanel", nullptr, window_flags)) {
        ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - topMargin * 2));
        ImGui::SetWindowPos(ImVec2(0, topMargin));
        if (ImGui::BeginChild("AdminPanelMenu", ImVec2(ImGui::GetIO().DisplaySize.x, 35), true)) {
            if (ImGui::Button("Users")) {
                fetchUsers = true;
                currentAdminScreen = AdminScreen::ADMIN_USERS;
            }
            ImGui::SameLine();
            if (ImGui::Button("Logs")) {
                fetchLogs = true;
                currentAdminScreen = AdminScreen::ADMIN_LOGS;
            }
        }
        ImGui::EndChild();

        auto childSize = ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - topMargin * 5);
        if (currentAdminScreen == AdminScreen::ADMIN_LOGS) {
            if (!logResponse.empty()) {
                float expectedSize = ((float)logResponse.size() * 70) + 35; // 70 = height of each log 35 = height of filter
                if (expectedSize > childSize.y) {
                    childSize.y = expectedSize;
                }
            }
        } else if (currentAdminScreen == AdminScreen::ADMIN_USERS) {
            if (!userResponse.empty()) {
                float expectedSize = ((float)userResponse.size() * 70) + 35; // 70 = height of each log 35 = height of filter
                if (expectedSize > childSize.y) {
                    childSize.y = expectedSize;
                }
            }
        }

        if (ImGui::BeginChild("AdminPanelDisplay", childSize, true)) {
            switch (currentAdminScreen) {
                case AdminScreen::ADMIN_USERS:
                    tryFetchUsers();
                    drawAdminUsers();
                    break;
                case AdminScreen::ADMIN_LOGS:
                    tryFetchLogs();
                    drawAdminLogs();
                    break;
            }
        }
        ImGui::EndChild();

        ImGui::End();
    }
}

void LibrarySystem::drawAdminLogs() {
    if (ImGui::BeginChild("AdminLogsFilter", ImVec2(ImGui::GetIO().DisplaySize.x, 35), true)) {
        if (ImGui::BeginChild("AdminLogsFilter__Amount", ImVec2(150, 25))) {
            ImGui::InputInt("Amount", &logAmount);
        }
        ImGui::EndChild();
        ImGui::SameLine();
        if (ImGui::BeginChild("AdminLogsFilter__User", ImVec2(150, 25))) {
            ImGui::InputInt("User", &logUserTarget);
        }
        ImGui::EndChild();
        ImGui::SameLine();
        if (ImGui::BeginChild("AdminLogsFilter__Page", ImVec2(150, 25))) {
            ImGui::InputInt("Page", &logPage);
        }
        ImGui::EndChild();
        ImGui::SameLine();
        if (ImGui::BeginChild("AdminLogsFilter__Fetch", ImVec2(150, 25))) {
            if (ImGui::Button("Fetch")) {
                logResponse.clear();
                fetchLogs = true;
            }
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();

    if (logResponse.empty()) {
        ImGui::Text("No logs found, Check filter or refresh in the case of an error.");
        ImGui::Text("(you can refresh by re-selecting the current tab or clicking the fetch button)");
    } else {
        for (auto& log : logResponse) {
            if (log["__time_str"].is_null()) {
                auto creation = date::year_month_day{date::floor<date::days>(std::chrono::system_clock::from_time_t(log["time"].get<int>()))};
                log["__time_str"] = std::string(date::format("%d/%m/%Y", creation));
            }
            if (ImGui::BeginChild(log["id"].get<int>(), ImVec2(ImGui::GetIO().DisplaySize.x, 65), true)) {
                ImGui::Text("User ID: %d | %s", log["user_id"].get<int>(), log["__time_str"].get<std::string>().c_str());
                ImGui::Text("Action: %s", log["action"].get<std::string>().c_str());
                ImGui::Text("%s", log["response"].get<std::string>().c_str());
            }
            ImGui::EndChild();
        }
    }
}

void LibrarySystem::drawAdminUsers() {

    if (ImGui::BeginChild("AdminUsersFilter", ImVec2(ImGui::GetIO().DisplaySize.x, 35), true)) {
        if (ImGui::BeginChild("AdminUsersFilter__Amount", ImVec2(150, 25))) {
            ImGui::InputInt("Amount", &userAmount);
        }
        ImGui::EndChild();
        ImGui::SameLine();
        if (ImGui::BeginChild("AdminUsersFilter__Page", ImVec2(150, 25))) {
            ImGui::InputInt("Page", &userPage);
        }
        ImGui::EndChild();
        ImGui::SameLine();
        if (ImGui::BeginChild("AdminUsersFilter__Fetch", ImVec2(150, 25))) {
            if (ImGui::Button("Fetch")) {
                userResponse.clear();
                fetchUsers = true;
            }
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();

    if (!fetchUsers) { // users have been fetched

        if (userResponse.empty()) {
            ImGui::Text("No users found, Check filter or refresh in the case of an error.");
            ImGui::Text("(you can refresh by re-selecting the current tab or clicking the fetch button)");
        }

        for (auto &user : userResponse){
            if (user["__time_str"].is_null()) {
                auto creation = date::year_month_day{date::floor<date::days>(std::chrono::system_clock::from_time_t(user["created_at"].get<int>()))};
                user["__time_str"] = std::string(date::format("%d/%m/%Y", creation));
            }
            if (ImGui::BeginChild(user["id"].get<int>(), ImVec2(ImGui::GetIO().DisplaySize.x, 65), true)) {
                ImGui::Text(" %d | %s", user["id"].get<int>(), user["username"].get<std::string>().c_str());
                ImGui::Text("Created: %s", user["__time_str"].get<std::string>().c_str());
                ImGui::Text("Admin: %s", (user["isAdmin"].get<bool>() ? "True" : "False"));
            }
            ImGui::EndChild();
        }

    }
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
        if (books["error"].get<std::string>() == "invalid token") {
            fetchedBooks = false;
            libraryLogin->queueLogout();
        }
        return;
    }

    if (books["books"].is_null() || books["books"].empty()) {
        std::printf("Error: No books found \n");
        return;
    }

    this->libraryBookCount = (int)books["books"].size();
    this->libraryBooks = new LibraryBook *[libraryBookCount];

    for (int i = 0; i < libraryBookCount; i++) {
        if (books["books"][i]["currentOwner"].is_null() || books["books"][i]["currentOwner"].get<int>() == 0) {
            libraryBooks[i] = new UnownedLibraryBook(books["books"][i]);
        } else {
            libraryBooks[i] = new OwnedLibraryBook(books["books"][i]);
        }
    }
    this->getLoginSystem()->getAccount()->updateBooks(this->libraryBooks, this->libraryBookCount);

    std::printf("Fetched %d book(s)\n", libraryBookCount);

}

void LibrarySystem::tryFetchLogs() {
    if (fetchLogs) {
        nlohmann::json request;
        request["authentication"]["username"] = libraryLogin->getAccount()->getUsername();
        request["authentication"]["token"] = libraryLogin->getAccount()->getToken();
        request["authentication"]["id"] = libraryLogin->getAccount()->getId();
        request["amount"] = logAmount;
        request["target_user"] = logUserTarget;
        request["page"] = logPage;
        nlohmann::json logs = WebRequest::fetchLogs(request);

        if (logs.empty()) {
            std::printf("Error fetching logs \n");
            fetchLogs = false;
            return;
        }

        if (!logs["error"].is_null() && !logs["error"].get<std::string>().empty()) {
            std::printf("Error: %s \n", logs["error"].get<std::string>().c_str());
            if (logs["error"].get<std::string>() == "invalid token") {
                fetchedBooks = false;
                libraryLogin->queueLogout();
            }
            fetchLogs = false;
            return;
        }

        if (logs["success"].is_null() || !logs["success"].get<bool>()) {
            std::printf("Unknown Error \n");
            fetchLogs = false;
            return;
        }

        if (logs["actions"].is_null() || logs["actions"].empty()) {
            std::printf("No logs found \n");
            fetchLogs = false;
            return;
        } else {
            logResponse = logs["actions"];
        }

        std::printf("Fetched (%zu) logs \n", logResponse.size());

        fetchLogs = false;
    }
}

void LibrarySystem::tryFetchUsers() {
    if (fetchUsers) {
        nlohmann::json request;
        request["authentication"]["username"] = libraryLogin->getAccount()->getUsername();
        request["authentication"]["token"] = libraryLogin->getAccount()->getToken();
        request["authentication"]["id"] = libraryLogin->getAccount()->getId();
        request["amount"] = userAmount;
        request["page"] = userPage;

        nlohmann::json users = WebRequest::fetchUsers(request);


        if (users.empty()) {
            std::printf("Error fetching users \n");
            fetchUsers = false;
            return;
        }

        if (!users["error"].is_null() && !users["error"].get<std::string>().empty()) {
            std::printf("Error: %s \n", users["error"].get<std::string>().c_str());
            if (users["error"].get<std::string>() == "invalid token") {
                fetchedBooks = false;
                libraryLogin->queueLogout();
            }
            fetchUsers = false;
            return;
        }

        if (users["success"].is_null() || !users["success"].get<bool>()) {
            std::printf("Unknown Error \n");
            fetchUsers = false;
            return;
        }

        if (users["users"].is_null() || users["users"].empty()) {
            std::printf("No users found \n");
            fetchUsers = false;
            return;
        } else {
            userResponse = users["users"];
        }

        std::printf("Fetched (%zu) users \n", userResponse.size());

        fetchUsers = false;

    }
}