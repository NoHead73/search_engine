cmake_minimum_required(VERSION 3.21)
project(search_engine)

set(CMAKE_CXX_STANDARD 17)

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
        main.cpp
        ConverterJSON.cpp
        InvertedIndex.cpp
        SearchServer.cpp
        Logger.h
)

target_link_libraries(search_engine PRIVATE
        nlohmann_json::nlohmann_json
)

if(GTest_FOUND)
    target_link_libraries(search_engine PRIVATE GTest::gtest GTest::gtest_main GTest::gmock)
    include(GoogleTest)
    gtest_discover_tests(search_engine)
elseif(googletest_POPULATED)
    target_link_libraries(search_engine PRIVATE gtest gtest_main gmock)
    include(GoogleTest)
    gtest_discover_tests(search_engine)
endif()