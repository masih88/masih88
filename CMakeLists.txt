cmake_minimum_required(VERSION 3.21)

project(ElektroRechner
    VERSION 1.9.0
    DESCRIPTION "Elektro- und GA-Rechner fuer Gebaeudeautomation und Industrie"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ELC_BUILD_APP   "Qt-Quick-App bauen (benoetigt Qt 6.4+)" ON)
option(ELC_BUILD_CLI   "Kommandozeilen-Tool elcalc bauen"       ON)
option(ELC_BUILD_TESTS "Unit-Tests bauen (Catch2 v3)"            ON)
option(ELC_SANITIZE    "AddressSanitizer + UBSan (GCC/Clang)"    OFF)

# Warnings as an interface target so every part of the project gets the same flags.
add_library(elc_warnings INTERFACE)
if(MSVC)
    # /utf-8 is required: sources contain Ω, φ, ², … in string literals
    target_compile_options(elc_warnings INTERFACE /W4 /permissive- /utf-8)
else()
    target_compile_options(elc_warnings INTERFACE -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor)
endif()

if(ELC_SANITIZE AND NOT MSVC)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
    add_link_options(-fsanitize=address,undefined)
endif()

add_subdirectory(core)

# CLI and tests only make sense on desktop platforms
if(ELC_BUILD_CLI AND NOT ANDROID AND NOT IOS)
    add_subdirectory(cli)
endif()

if(ELC_BUILD_TESTS AND NOT ANDROID AND NOT IOS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(ELC_BUILD_APP)
    find_package(Qt6 6.4 QUIET COMPONENTS Quick QuickControls2)
    if(Qt6_FOUND)
        add_subdirectory(app)
    else()
        message(WARNING "Qt 6 nicht gefunden – App wird nicht gebaut (nur Rechenkern/CLI/Tests). "
                        "Mit Qt Creator oeffnen oder CMAKE_PREFIX_PATH auf Qt setzen.")
    endif()
endif()
