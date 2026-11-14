#include <catch2/catch_test_macros.hpp>

#include "astracare/report.h"

using namespace astracare;

TEST_CASE("Medical reports contain complete escaped JSON data") {
    MedicalReport report;
    report.generatedAt = "2026-11-15T10:00:00Z";
    report.notes = "offline\"mission";
    report.guidanceIds = {"chest-pain"};
    VitalReading reading;
    reading.id = 7;
    reading.timestamp = "2026-11-15T09:00:00Z";
    reading.heartRate = 72;
    reading.systolic = 120;
    reading.diastolic = 80;
    reading.sleepHours = 7.0;
    reading.mood = 4;
    reading.symptoms = "None";
    report.readings.push_back(reading);
    report.alerts.push_back({AlertLevel::Caution, "Watch", "Review", "chest-pain"});

    const auto json = ReportBuilder::toJson(report);
    REQUIRE(json.find("\"heartRate\": 72") != std::string::npos);
    REQUIRE(json.find("\"level\": \"Caution\"") != std::string::npos);
    REQUIRE(json.find("offline\\\"mission") != std::string::npos);
}

TEST_CASE("CSV reports include a stable header and every reading") {
    MedicalReport report;
    VitalReading reading;
    reading.id = 2;
    reading.timestamp = "2026-11-15T09:00:00Z";
    reading.heartRate = 72;
    reading.symptoms = "Headache";
    report.readings.push_back(reading);

    const auto csv = ReportBuilder::toCsv(report);
    REQUIRE(csv.find("id,timestamp,heartRate,systolic,diastolic,sleepHours,mood,symptoms") == 0);
    REQUIRE(csv.find("2,\"2026-11-15T09:00:00Z\",72") != std::string::npos);
}
