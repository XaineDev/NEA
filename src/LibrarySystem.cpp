//
// Created by Xaine on 28/03/2023.
//

#include "LibrarySystem.h"
#include "external/imgui.h"

LibrarySystem::LibrarySystem(HWND window) {
    this->window = window;
}

void LibrarySystem::drawLibraryScreen() {
    if (!libraryLogin.isLoggedIn())
    {
        if (ImGui::Begin("Login", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove))
        {
            auto windowSize = ImVec2(350, 150);
            ImGui::SetWindowSize(windowSize);
            RECT rect;
            GetWindowRect(window, &rect);
            auto windowPos = ImVec2(((float)rect.right - (float)rect.left) / 2 - windowSize.x / 2, ((float)rect.bottom - (float)rect.top) / 2 - windowSize.y / 2);
            ImGui::SetWindowPos(windowPos);
            if (libraryLogin.isLoggingIn())
            {
                ImGui::Text("Logging in...");
            } else {
                if (libraryLogin.isLoginFailed() && libraryLogin.timeSinceLoginFailed() < 2)
                {
                    ImGui::Text("Login failed. Please try again.");
                } else {
                    ImGui::Text("Please login to continue");
                    ImGui::InputText("Username", libraryLogin.username, 128);
                    ImGui::InputText("Password", libraryLogin.password, 128, ImGuiInputTextFlags_Password);
                    if (ImGui::Button("Login"))
                    {
                        libraryLogin.login();
                    }
                }
            }

            ImGui::End();
        }
        return;
    }

    // create imgui main menu bar with clickable buttons
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::MenuItem("Main Menu"))
        {
            std::cout << "Main Menu" << std::endl;
        }
        ImGui::EndMainMenuBar();
    }
}
