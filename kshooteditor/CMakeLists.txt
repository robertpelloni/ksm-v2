cmake_minimum_required(VERSION 3.20)
project(kshooteditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../kshootmania/ThirdParty/NocoUI/cmake/macos")
endif()

if(NOT HEADLESS_BUILD)
    find_package(Siv3D REQUIRED)
endif()

add_executable(kshooteditor src/EditorMain.cpp)

if(NOT HEADLESS_BUILD)
    target_link_libraries(kshooteditor PRIVATE
        Siv3D::Siv3D
        NocoUI
    )
else()
    target_compile_definitions(kshooteditor PRIVATE HEADLESS_BUILD)
endif()
