cmake_minimum_required(VERSION 3.24)
project(NEA)

set(CMAKE_CXX_STANDARD 23)

add_executable(NEA src/main.cpp src/external/imgui.cpp src/external/imgui_demo.cpp src/external/imgui_draw.cpp src/external/imgui_impl_dx9.cpp src/external/imgui_impl_win32.cpp src/external/imgui_internal.h src/external/imgui_tables.cpp src/external/imgui_widgets.cpp src/external/imstb_rectpack.h src/external/imstb_textedit.h src/external/imstb_truetype.h src/LibrarySystem.cpp src/LibraryLogin.cpp src/LibraryAccount.h src/LibraryAccount.cpp src/net/WebRequest.cpp src/net/WebRequest.h src/net/HTTPRequest.hpp src/constants.h src/LibraryBook.h src/LibraryBook.cpp src/external/time/tz.cpp src/external/time/date/date.h)

target_link_libraries(NEA d3dx9 d3d9 dwmapi ws2_32)