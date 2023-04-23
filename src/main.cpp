// Dear ImGui: standalone application for DirectX 9
// https://github.com/ocornut/imgui/tree/master/docs

#include "external/imgui.h"
#include "external/imgui_impl_dx9.h"
#include "external/imgui_impl_win32.h"
#include "LibrarySystem.h"
#include "net/WebRequest.h"
#include <d3d9.h>
#include <d3dx9tex.h>
#include <tchar.h>
#include <iostream>

// Programs Data
static LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
static LPDIRECT3D9 g_pD3D = nullptr;
static D3DPRESENT_PARAMETERS g_d3dpp = {};
static LibrarySystem *librarySystem;

// Forward declarations of DirectX / Windows API helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*
 * Load a texture from disk into a new DirectX texture
 */
bool LoadTextureFromFile(const char *filename, PDIRECT3DTEXTURE9 *out_texture, int *out_width, int *out_height) {
    // Load texture from disk
    PDIRECT3DTEXTURE9 texture;
    HRESULT hr = D3DXCreateTextureFromFileA(g_pd3dDevice, filename, &texture);
    if (hr != S_OK)
        return false;

    // Retrieve description of the texture surface so we can access its size
    D3DSURFACE_DESC my_image_desc;
    texture->GetLevelDesc(0, &my_image_desc);

    *out_texture = texture;
    *out_width = (int) my_image_desc.Width;
    *out_height = (int) my_image_desc.Height;
    return true;
}

// Main code
int main(int, char **) {
    // get icon
    auto hIcon = (HICON) LoadImage( // returns a HANDLE so we have to cast to HICON
            nullptr,             // hInstance must be NULL when loading from a file
            R"(..\assets\book.ico)",   // the icon file name
            IMAGE_ICON,       // specifies that the file is an icon
            0,                // width of the image (we'll specify default later on)
            0,                // height of the image
            LR_LOADFROMFILE |  // we want to load a file (as opposed to a resource)
            LR_DEFAULTSIZE |   // default metrics based on the type (IMAGE_ICON, 32x32)
            LR_SHARED         // let the system release the handle when it's no longer used
    );

    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = {sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), hIcon, nullptr, nullptr,
                      nullptr, L"Library System", hIcon};
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Library System",
                                WS_OVERLAPPED + WS_THICKFRAME + WS_BORDER + WS_SYSMENU, 100, 100, 1280, 800, nullptr,
                                nullptr, wc.hInstance, nullptr);



    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Font
    io.Fonts->AddFontFromFileTTF("..\\assets\\SFMonoRegular.otf", 13.0f);

    // window state
    ImVec4 clear_color = ImVec4(0.106f, 0.106f, 0.106f, 1.00f); // background colour for the window

    // load default profile picture
    int my_image_width = 0;
    int my_image_height = 0;
    PDIRECT3DTEXTURE9 my_texture = NULL;
    bool ret = LoadTextureFromFile("..\\assets\\default.jpg", &my_texture, &my_image_width, &my_image_height);
    if (!ret) {
        std::cout << "Failed to load default profile picture" << std::endl;
    } else {
        std::cout << "Loaded default profile picture" << std::endl;
    }

    bool shouldClose = false;
    bool showingConsole = false;
    bool toggledConsole = false;
    librarySystem = new LibrarySystem(hwnd, Image(my_texture, my_image_width / 5, my_image_height / 5));

    // add rounding to windows created by imgui
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5);

    while (true) {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                shouldClose = true;
        }

        if (shouldClose) {
            // if close is queued, skip rendering the next frame and exit the loop
            break;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        librarySystem->drawLibraryScreen();

        // check if control shift o is pressed and toggle showingConsole
        if (io.KeyCtrl && io.KeyShift && io.KeysDown[79]) {
            if (!toggledConsole) {
                showingConsole = !showingConsole;
                toggledConsole = true;
            }
        } else {
            toggledConsole = false;
        }

        if (showingConsole) {
            ImGui::ShowDemoWindow(&showingConsole);
        }


        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int) (clear_color.x * clear_color.w * 255.0f),
                                              (int) (clear_color.y * clear_color.w * 255.0f),
                                              (int) (clear_color.z * clear_color.w * 255.0f),
                                              (int) (clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    delete librarySystem;

    // shutdown imgui and clean up resources
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd) {
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp,
                             &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D() {
    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
    if (g_pD3D) {
        g_pD3D->Release();
        g_pD3D = nullptr;
    }
}

void ResetDevice() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
        case WM_SIZE:
            if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED) {
                g_d3dpp.BackBufferWidth = LOWORD(lParam);
                g_d3dpp.BackBufferHeight = HIWORD(lParam);
                ResetDevice();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}