cmake_minimum_required(VERSION 3.14)
project(ATMApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Main application ---
add_executable(atm_app src/main.cpp)
target_link_libraries(atm_app mysqlcppconn)

# --- Unit tests ---
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    add_executable(atm_tests
        tests/test_account.cpp
        tests/test_login.cpp
        tests/test_customer_options.cpp
        tests/test_admin_options.cpp
    )

    target_include_directories(atm_tests PRIVATE src)
    target_link_libraries(atm_tests GTest::GTest GTest::Main pthread)

    # Coverage flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(atm_tests PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(atm_tests PRIVATE --coverage)
    endif()

    add_test(NAME ATMTests COMMAND atm_tests --gtest_output=xml:test_results.xml)
endif()

# --- Doxygen documentation ---
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating documentation with Doxygen"
    )
endif()
