cmake_minimum_required(VERSION 3.16)
project(kshootmania CXX)

if (NOT CMAKE_CONFIGURATION_TYPES AND
    NOT CMAKE_NO_BUILD_TYPE AND
    NOT CMAKE_BUILD_TYPE AND
    CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(STATUS "[!] Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release)
endif()

option(OUTPUT_LICENSE_TXT "Output license.txt on startup" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/App)

file(GLOB_RECURSE KSM_SRC_FILES src/*.cpp)
list(REMOVE_ITEM KSM_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.cpp)

add_executable(kshootmania
  ${KSM_SRC_FILES}
  )

if(APPLE)
    set_target_properties(kshootmania PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "K-Shoot MANIA"
        MACOSX_BUNDLE_BUNDLE_VERSION "2.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "2.0.0-alpha3"
        MACOSX_BUNDLE_ICON_FILE icon.icns
    )
    
    set(APP_BUNDLE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kshootmania.app)

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/App/engine")
        # engineディレクトリ全体をコピー
        add_custom_command(TARGET kshootmania POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/App/engine
            ${APP_BUNDLE_PATH}/Contents/Resources/engine
        )

        # engine/font/min以外のフォントフォルダを削除
        add_custom_command(TARGET kshootmania POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rm -rf
            ${APP_BUNDLE_PATH}/Contents/Resources/engine/font/fontawesome
            ${APP_BUNDLE_PATH}/Contents/Resources/engine/font/materialdesignicons
            ${APP_BUNDLE_PATH}/Contents/Resources/engine/font/mplus
            ${APP_BUNDLE_PATH}/Contents/Resources/engine/font/noto-cjk
            ${APP_BUNDLE_PATH}/Contents/Resources/engine/font/noto-emoji
        )

        # engine/soundfontフォルダを削除
        add_custom_command(TARGET kshootmania POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rm -rf
            ${APP_BUNDLE_PATH}/Contents/Resources/engine/soundfont
        )
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/App/assets")
        add_custom_command(TARGET kshootmania POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/App/assets
            ${APP_BUNDLE_PATH}/Contents/Resources/assets
        )
    endif()
    
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/App/icon.icns)
        set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/App/icon.icns PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
        )
        target_sources(kshootmania PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/App/icon.icns)
    endif()
    
    add_custom_command(TARGET kshootmania POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        ${APP_BUNDLE_PATH}/Contents/Frameworks
    )
    
    add_custom_command(TARGET kshootmania POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/ksmaudio/ThirdParty_macOS/bass/libbass.dylib
        ${APP_BUNDLE_PATH}/Contents/Frameworks/libbass.dylib
    )
    
    add_custom_command(TARGET kshootmania POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/ksmaudio/ThirdParty_macOS/bass_fx/libbass_fx.dylib
        ${APP_BUNDLE_PATH}/Contents/Frameworks/libbass_fx.dylib
    )
    
    set_target_properties(kshootmania PROPERTIES
        BUILD_RPATH "@executable_path/../Frameworks"
        INSTALL_RPATH "@executable_path/../Frameworks"
    )
endif()

target_include_directories(kshootmania PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/CoTaskLib/include
)

find_package(Siv3D REQUIRED)
target_link_libraries(kshootmania PRIVATE
    Siv3D::Siv3D
    kson
    ksmaudio
    NocoUI
)

if(APPLE)
	target_link_libraries(kshootmania PRIVATE ksmplatform_macos)
endif()

target_compile_features(kshootmania PRIVATE cxx_std_20)

if(OUTPUT_LICENSE_TXT)
	target_compile_definitions(kshootmania PRIVATE OUTPUT_LICENSE_TXT)
endif()

if(APPLE)
    # Siv3DのmacOS/Linux版はプリコンパイル済みヘッダ非対応のため通常のヘッダとしてインクルード
    target_compile_options(kshootmania PRIVATE
        -Wall
        -Wextra
        -Wno-unknown-pragmas
        -include ${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.h
    )
    
    target_compile_definitions(kshootmania PRIVATE
        SIV3D_PLATFORM_MACOS
    )
elseif(UNIX)
    # Siv3DのmacOS/Linux版はプリコンパイル済みヘッダ非対応のため通常のヘッダとしてインクルード
    target_compile_options(kshootmania PRIVATE
        -Wall
        -Wextra
        -Wno-unknown-pragmas
        -include ${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.h
    )

    # BASSライブラリへのRPATHを設定
    set_target_properties(kshootmania PROPERTIES
        BUILD_RPATH "${CMAKE_SOURCE_DIR}/ksmaudio/ThirdParty_Linux/bass:${CMAKE_SOURCE_DIR}/ksmaudio/ThirdParty_Linux/bass_fx"
        INSTALL_RPATH "$ORIGIN/../lib"
    )
endif()
