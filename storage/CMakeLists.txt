add_library(astracare_storage src/database.cpp)
add_library(astracare::storage ALIAS astracare_storage)

target_include_directories(astracare_storage PUBLIC include)
target_link_libraries(astracare_storage PUBLIC astracare::core)

# TODO(Role 6): link SQLite
#   find_package(SQLite3 REQUIRED)
#   target_link_libraries(astracare_storage PUBLIC SQLite::SQLite3)`