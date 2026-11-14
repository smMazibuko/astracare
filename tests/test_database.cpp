#include <catch2/catch_test_macros.hpp>

#include <cstdio>

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

TEST_CASE("SQLite readings survive closing and reopening a file database") {
    const std::string path = "astracare_database_test.sqlite";
    {
        Database db;
        REQUIRE(db.open(path));
        VitalReading reading;
        reading.timestamp = "2026-11-15T10:00:00Z";
        reading.heartRate = 68;
        reading.systolic = 118;
        reading.diastolic = 76;
        reading.sleepHours = 7.5;
        reading.mood = 4;
        reading.symptoms = "None";
        REQUIRE(db.insertReading(reading) > 0);
    }
    {
        Database db;
        REQUIRE(db.open(path));
        const auto readings = db.allReadings();
        REQUIRE(readings.size() == 1);
        REQUIRE(readings.front().heartRate == 68);
        REQUIRE(readings.front().symptoms == "None");
    }
    std::remove(path.c_str());
}
