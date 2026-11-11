include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(astracare_tests
    test_alert_engine.cpp
    test_database.cpp
)
target_link_libraries(astracare_tests PRIVATE
    Catch2::Catch2WithMain
    astracare::core
    astracare::storage
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(astracare_tests)
