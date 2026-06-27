cmake_minimum_required(VERSION 3.16)
project(kshooteditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(kshooteditor src/EditorMain.cpp)

if(NOT HEADLESS_BUILD)
    # Link against OpenSiv3D and NocoUI when available
    # target_link_libraries(kshooteditor PRIVATE Siv3D NocoUI)
endif()
