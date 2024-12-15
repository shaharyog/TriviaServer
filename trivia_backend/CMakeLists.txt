cmake_minimum_required(VERSION 3.5...3.27)
project(trivia_backend)

set(CMAKE_CXX_STANDARD 17)
file(
    GLOB_RECURSE
    SOURCES
    "src/*.cpp"
    "src/*.h"
    "src/*.hpp"
)
add_executable(trivia_backend ${SOURCES})

if (WIN32)
    # Link ws2_32 and bcrypt libraries on Windows
    target_link_libraries(trivia_backend PRIVATE ws2_32 bcrypt)
endif ()

find_path(KISSNET_INCLUDE_DIRS "kissnet.hpp")
target_include_directories(trivia_backend PRIVATE ${KISSNET_INCLUDE_DIRS})

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(trivia_backend PRIVATE nlohmann_json::nlohmann_json)

find_package(SqliteOrm CONFIG REQUIRED)
target_link_libraries(trivia_backend PRIVATE sqlite_orm::sqlite_orm)

find_package(httplib CONFIG REQUIRED)
target_link_libraries(trivia_backend PRIVATE httplib::httplib)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(trivia_backend PRIVATE fmt::fmt-header-only)

find_package(date CONFIG REQUIRED)
target_link_libraries(trivia_backend PRIVATE date::date date::date-tz)