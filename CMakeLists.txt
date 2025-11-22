cmake_minimum_required(VERSION 3.21)
project(search_engine)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY /lib)

enable_testing()

find_package(GTest)
if(NOT GTest_FOUND)
    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
endif()

include(FetchContent)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

add_executable(search_engine
        src/main.cpp
        src/ConverterJSON.cpp
        src/InvertedIndex.cpp
        src/SearchServer.cpp
)

target_include_directories(search_engine PRIVATE include)

target_link_libraries(search_engine PRIVATE
        nlohmann_json::nlohmann_json
)

add_executable(test_search_engine
        tests/test_search_engine.cpp
        src/ConverterJSON.cpp
        src/InvertedIndex.cpp
        src/SearchServer.cpp
)

target_include_directories(test_search_engine PRIVATE include)

if(GTest_FOUND)
    target_link_libraries(test_search_engine PRIVATE GTest::gtest GTest::gtest_main GTest::gmock nlohmann_json::nlohmann_json)
    include(GoogleTest)
    gtest_discover_tests(test_search_engine)
elseif(googletest_POPULATED)
    target_link_libraries(test_search_engine PRIVATE gtest gtest_main gmock nlohmann_json::nlohmann_json)
    include(GoogleTest)
    gtest_discover_tests(test_search_engine)
endif()
