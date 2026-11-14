# JSON parsing (used by the alert engine to read resources/ranges.json).
# The guard lets other modules declare nlohmann_json too without a clash.
if(NOT TARGET nlohmann_json::nlohmann_json)
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

add_library(astracare_core
    src/alert_engine.cpp
    src/guidance.cpp
    src/report.cpp
    src/emergency.cpp
)
add_library(astracare::core ALIAS astracare_core)

target_include_directories(astracare_core PUBLIC include)
target_compile_features(astracare_core PUBLIC cxx_std_17)
target_link_libraries(astracare_core PRIVATE nlohmann_json::nlohmann_json)
