add_library(astracare_storage src/database.cpp)
add_library(astracare::storage ALIAS astracare_storage)

target_include_directories(astracare_storage PUBLIC include)
target_link_libraries(astracare_storage PUBLIC astracare::core)

find_package(SQLite3 QUIET)
if(SQLite3_FOUND)
    target_link_libraries(astracare_storage PUBLIC SQLite::SQLite3)
else()
    # Keep Windows and other developer machines self-contained when no
    # system SQLite development package is installed.
    include(FetchContent)
    FetchContent_Declare(
        sqlite3
        URL https://www.sqlite.org/2024/sqlite-amalgamation-3450300.zip
    )
    FetchContent_MakeAvailable(sqlite3)
    add_library(astracare_sqlite STATIC "${sqlite3_SOURCE_DIR}/sqlite3.c")
    target_include_directories(astracare_sqlite PUBLIC "${sqlite3_SOURCE_DIR}")
    target_compile_definitions(astracare_sqlite PRIVATE SQLITE_THREADSAFE=1)
    target_link_libraries(astracare_storage PUBLIC astracare_sqlite)
endif()
