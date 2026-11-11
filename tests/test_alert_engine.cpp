#include <catch2/catch_test_macros.hpp>

#include "astracare/alert_engine.h"

using namespace astracare;

TEST_CASE("Normal reading produces no alerts") {
    AlertEngine engine;
    VitalReading r;
    r.heartRate = 70;
    REQUIRE(engine.evaluate(r).empty());
}

TEST_CASE("Very high heart rate produces an urgent alert") {
    AlertEngine engine;
    VitalReading r;
    r.heartRate = 150;
    auto alerts = engine.evaluate(r);
    REQUIRE(alerts.size() == 1);
    REQUIRE(alerts[0].level == AlertLevel::Urgent);
}
