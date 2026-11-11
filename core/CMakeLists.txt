add_library(astracare_core
    src/alert_engine.cpp
    src/guidance.cpp
    src/report.cpp
)
add_library(astracare::core ALIAS astracare_core)

target_include_directories(astracare_core PUBLIC include)
target_compile_features(astracare_core PUBLIC cxx_std_17)
