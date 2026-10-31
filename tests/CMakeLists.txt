find_package(Catch2 3 QUIET)
if(NOT Catch2_FOUND)
    include(FetchContent)
    FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.4)
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif()

add_executable(elc_tests
    test_basics.cpp
    test_cables.cpp
    test_motors_industry.cpp
    test_automation.cpp
    test_electronics_units.cpp
    test_catalog.cpp
    test_facilities.cpp
    test_lists.cpp
)
target_link_libraries(elc_tests PRIVATE elc::core elc_warnings Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(elc_tests)
