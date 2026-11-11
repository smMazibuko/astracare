#include <catch2/catch_test_macros.hpp>

#include "astracare/database.h"

using namespace astracare;

TEST_CASE("Inserted readings can be read back") {
    Database db;
    REQUIRE(db.open(":memory:"));

    VitalReading r;
    r.heartRate = 72;
    int id = db.insertReading(r);

    REQUIRE(id > 0);
    REQUIRE(db.allReadings().size() == 1);
}
