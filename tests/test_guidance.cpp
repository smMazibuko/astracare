#include <catch2/catch_test_macros.hpp>

#include "astracare/guidance.h"

using namespace astracare;

TEST_CASE("Bundled emergency procedures load into the emergency guidance library") {
    GuidanceLibrary library;
    REQUIRE(library.loadEmergencyFromFile(std::string(ASTRACARE_RESOURCE_DIR) + "/guidance/emergency.json"));
    REQUIRE(library.all().size() == 3);
    REQUIRE(library.all().front().category == "emergency");
    REQUIRE(library.findById("emergency001").has_value());
    REQUIRE(library.findById("emergency001")->steps.find("1. Abort the spacewalk.") != std::string::npos);
}
