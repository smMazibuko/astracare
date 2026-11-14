#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "astracare/alert_engine.h"

using namespace astracare;

namespace {

// A healthy, fully filled-in reading.
VitalReading normalReading() {
    VitalReading r;
    r.heartRate = 70;
    r.systolic = 115;
    r.diastolic = 75;
    r.sleepHours = 7.5;
    r.mood = 4;
    return r;
}

VitalReading withHeartRate(int bpm) {
    VitalReading r;
    r.heartRate = bpm;
    return r;
}

VitalReading withSymptoms(const std::string& text) {
    VitalReading r = normalReading();
    r.symptoms = text;
    return r;
}

// A reading on a given day with only the fields a trend test cares about.
VitalReading day(int d, int heartRate, int systolic, double sleep, int mood) {
    VitalReading r;
    r.timestamp = "2026-11-" + std::string(d < 10 ? "0" : "") + std::to_string(d) + "T08:00:00Z";
    r.heartRate = heartRate;
    r.systolic = systolic;
    r.sleepHours = sleep;
    r.mood = mood;
    return r;
}

bool hasTitle(const std::vector<Alert>& alerts, const std::string& title) {
    for (const auto& a : alerts) {
        if (a.title == title)
            return true;
    }
    return false;
}

// Writes `content` to a temporary file and removes it when it goes out of scope.
struct TempFile {
    std::filesystem::path path;
    explicit TempFile(const std::string& content, const std::string& name = "astracare_test.json")
        : path(std::filesystem::temp_directory_path() / name) {
        std::ofstream(path) << content;
    }
    ~TempFile() { std::filesystem::remove(path); }
};

} // namespace

// ---------------------------------------------------------------------------
// Single readings
// ---------------------------------------------------------------------------

TEST_CASE("Normal reading produces no alerts") {
    AlertEngine engine;
    VitalReading r;
    r.heartRate = 70;
    REQUIRE(engine.evaluate(r).empty());
    REQUIRE(engine.evaluate(normalReading()).empty());
}

TEST_CASE("Very high heart rate produces an urgent alert") {
    AlertEngine engine;
    VitalReading r;
    r.heartRate = 150;
    auto alerts = engine.evaluate(r);
    REQUIRE(alerts.size() == 1);
    REQUIRE(alerts[0].level == AlertLevel::Urgent);
    REQUIRE(alerts[0].guidanceId == guidance_ids::kHeartRate);
}

TEST_CASE("A completely empty reading produces no alerts") {
    AlertEngine engine;
    REQUIRE(engine.evaluate(VitalReading{}).empty());
}

TEST_CASE("Heart rate thresholds") {
    AlertEngine engine;

    SECTION("edges of the normal range are fine") {
        REQUIRE(engine.evaluate(withHeartRate(50)).empty());
        REQUIRE(engine.evaluate(withHeartRate(100)).empty());
    }
    SECTION("just outside normal is a caution") {
        auto high = engine.evaluate(withHeartRate(101));
        REQUIRE(high.size() == 1);
        REQUIRE(high[0].level == AlertLevel::Caution);
        REQUIRE(high[0].title == "High heart rate");

        auto low = engine.evaluate(withHeartRate(49));
        REQUIRE(low.size() == 1);
        REQUIRE(low[0].level == AlertLevel::Caution);
        REQUIRE(low[0].title == "Low heart rate");
    }
    SECTION("the caution band ends at 130 and 40") {
        REQUIRE(engine.evaluate(withHeartRate(130))[0].level == AlertLevel::Caution);
        REQUIRE(engine.evaluate(withHeartRate(40))[0].level == AlertLevel::Caution);
        REQUIRE(engine.evaluate(withHeartRate(131))[0].level == AlertLevel::Urgent);
        REQUIRE(engine.evaluate(withHeartRate(39))[0].level == AlertLevel::Urgent);
    }
    SECTION("the message mentions the value") {
        auto alerts = engine.evaluate(withHeartRate(135));
        REQUIRE(alerts[0].message.find("135") != std::string::npos);
    }
}

TEST_CASE("Blood pressure is reported as one alert") {
    AlertEngine engine;
    VitalReading r = normalReading();

    SECTION("both numbers high gives a single alert") {
        r.systolic = 150;
        r.diastolic = 95;
        auto alerts = engine.evaluate(r);
        REQUIRE(alerts.size() == 1);
        REQUIRE(alerts[0].level == AlertLevel::Caution);
        REQUIRE(alerts[0].title == "High blood pressure");
        REQUIRE(alerts[0].guidanceId == guidance_ids::kBloodPressure);
        REQUIRE(alerts[0].message.find("150/95") != std::string::npos);
    }
    SECTION("the more severe of the two numbers wins") {
        r.systolic = 185;
        r.diastolic = 95;
        auto alerts = engine.evaluate(r);
        REQUIRE(alerts.size() == 1);
        REQUIRE(alerts[0].level == AlertLevel::Urgent);
    }
    SECTION("low blood pressure") {
        r.systolic = 85;
        r.diastolic = 55;
        auto alerts = engine.evaluate(r);
        REQUIRE(alerts.size() == 1);
        REQUIRE(alerts[0].title == "Low blood pressure");
        REQUIRE(alerts[0].level == AlertLevel::Caution);

        r.systolic = 75;
        REQUIRE(engine.evaluate(r)[0].level == AlertLevel::Urgent);
    }
    SECTION("a missing diastolic value is skipped, not flagged") {
        r.diastolic = 0;
        REQUIRE(engine.evaluate(r).empty());
        r.systolic = 150;
        REQUIRE(engine.evaluate(r).size() == 1);
    }
}

TEST_CASE("Sleep and mood") {
    AlertEngine engine;
    VitalReading r = normalReading();

    SECTION("short sleep") {
        r.sleepHours = 5.0;
        auto alerts = engine.evaluate(r);
        REQUIRE(alerts.size() == 1);
        REQUIRE(alerts[0].level == AlertLevel::Caution);
        REQUIRE(alerts[0].guidanceId == guidance_ids::kSleep);

        r.sleepHours = 3.5;
        REQUIRE(engine.evaluate(r)[0].level == AlertLevel::Urgent);
    }
    SECTION("too much sleep is only a caution") {
        r.sleepHours = 11.0;
        auto alerts = engine.evaluate(r);
        REQUIRE(alerts.size() == 1);
        REQUIRE(alerts[0].title == "Long sleep");
        REQUIRE(alerts[0].level == AlertLevel::Caution);
    }
    SECTION("low mood is a caution, never urgent on its own") {
        r.mood = 2;
        REQUIRE(engine.evaluate(r)[0].level == AlertLevel::Caution);
        r.mood = 1;
        REQUIRE(engine.evaluate(r)[0].level == AlertLevel::Caution);
        r.mood = 3;
        REQUIRE(engine.evaluate(r).empty());
    }
    SECTION("an out-of-range mood is treated as not recorded") {
        r.mood = 0;
        REQUIRE(engine.evaluate(r).empty());
        r.mood = 9;
        REQUIRE(engine.evaluate(r).empty());
    }
}

// ---------------------------------------------------------------------------
// Symptoms
// ---------------------------------------------------------------------------

TEST_CASE("Chest pain is urgent and points at the emergency procedure") {
    AlertEngine engine;
    auto alerts = engine.evaluate(withSymptoms("Sharp chest pain"));
    REQUIRE(alerts.size() == 1);
    REQUIRE(alerts[0].level == AlertLevel::Urgent);
    REQUIRE(alerts[0].guidanceId == guidance_ids::kChestPain);
}

TEST_CASE("Symptom matching ignores case") {
    AlertEngine engine;
    REQUIRE(engine.evaluate(withSymptoms("CHEST PAIN"))[0].level == AlertLevel::Urgent);
    REQUIRE(engine.evaluate(withSymptoms("Shortness Of Breath"))[0].guidanceId ==
            guidance_ids::kBreathing);
}

TEST_CASE("Comma-separated tags are understood") {
    AlertEngine engine;
    auto alerts = engine.evaluate(withSymptoms("headache, dizziness, blurred vision"));
    REQUIRE(alerts.size() == 3);
    REQUIRE(hasTitle(alerts, "Dizziness reported"));
    REQUIRE(hasTitle(alerts, "Vision changes reported"));
    REQUIRE(hasTitle(alerts, "Feeling unwell"));
}

TEST_CASE("Several keywords for the same rule give one alert") {
    AlertEngine engine;
    auto alerts = engine.evaluate(withSymptoms("dizzy, lightheaded, vertigo"));
    REQUIRE(alerts.size() == 1);
}

TEST_CASE("Unrecognised or empty symptoms produce nothing") {
    AlertEngine engine;
    REQUIRE(engine.evaluate(withSymptoms("")).empty());
    REQUIRE(engine.evaluate(withSymptoms("sore finger")).empty());
}

TEST_CASE("Demo scenario: dizzy and tired with a fast heart rate") {
    AlertEngine engine;
    VitalReading r = normalReading();
    r.heartRate = 115;
    r.symptoms = "dizzy, tired";

    auto alerts = engine.evaluate(r);
    REQUIRE(alerts.size() == 3);
    REQUIRE(hasTitle(alerts, "High heart rate"));
    REQUIRE(hasTitle(alerts, "Dizziness reported"));
    REQUIRE(hasTitle(alerts, "Tiredness reported"));
    REQUIRE(highestLevel(alerts) == AlertLevel::Caution);
}

TEST_CASE("Alerts are sorted most severe first") {
    AlertEngine engine;
    VitalReading r = normalReading();
    r.heartRate = 105;                          // caution
    r.symptoms = "tired, difficulty breathing"; // caution + urgent

    auto alerts = engine.evaluate(r);
    REQUIRE(alerts.size() == 3);
    REQUIRE(alerts.front().level == AlertLevel::Urgent);
    REQUIRE(alerts.back().level == AlertLevel::Caution);
}

TEST_CASE("highestLevel and toString") {
    REQUIRE(highestLevel({}) == AlertLevel::Normal);

    std::vector<Alert> alerts(2);
    alerts[0].level = AlertLevel::Caution;
    alerts[1].level = AlertLevel::Urgent;
    REQUIRE(highestLevel(alerts) == AlertLevel::Urgent);

    REQUIRE(std::string(toString(AlertLevel::Normal)) == "Normal");
    REQUIRE(std::string(toString(AlertLevel::Caution)) == "Caution");
    REQUIRE(std::string(toString(AlertLevel::Urgent)) == "Urgent");
}

// ---------------------------------------------------------------------------
// Trends
// ---------------------------------------------------------------------------

TEST_CASE("Stable history produces no trend alerts") {
    AlertEngine engine;
    std::vector<VitalReading> history = {day(1, 70, 115, 7.5, 4), day(2, 72, 116, 7.0, 4),
                                         day(3, 71, 114, 7.5, 4), day(4, 70, 115, 7.2, 4)};
    REQUIRE(engine.evaluateTrend(history).empty());
}

TEST_CASE("Too little history produces no trend alerts") {
    AlertEngine engine;
    REQUIRE(engine.evaluateTrend({}).empty());
    REQUIRE(engine.evaluateTrend({day(1, 70, 115, 4.0, 1)}).empty());
    // Only two poor-sleep nights, the default streak is three.
    REQUIRE(engine.evaluateTrend({day(1, 70, 115, 5.0, 4), day(2, 70, 115, 5.0, 4)}).empty());
}

TEST_CASE("Poor sleep for several readings in a row") {
    AlertEngine engine;
    std::vector<VitalReading> history = {day(1, 70, 115, 7.5, 4), day(2, 70, 115, 5.5, 4),
                                         day(3, 70, 115, 5.0, 4), day(4, 70, 115, 5.8, 4)};
    auto alerts = engine.evaluateTrend(history);
    REQUIRE(alerts.size() == 1);
    REQUIRE(alerts[0].title == "Ongoing poor sleep");
    REQUIRE(alerts[0].guidanceId == guidance_ids::kSleep);

    // A good night in the middle of the last three breaks the streak.
    history[2].sleepHours = 7.5;
    REQUIRE(engine.evaluateTrend(history).empty());
}

TEST_CASE("Rising heart rate") {
    AlertEngine engine;
    std::vector<VitalReading> history = {day(1, 68, 115, 7.5, 4), day(2, 72, 115, 7.5, 4),
                                         day(3, 78, 115, 7.5, 4), day(4, 86, 115, 7.5, 4)};
    auto alerts = engine.evaluateTrend(history);
    REQUIRE(alerts.size() == 1);
    REQUIRE(alerts[0].title == "Heart rate trending up");
    REQUIRE(alerts[0].guidanceId == guidance_ids::kHeartRate);

    // A drop in the middle means it is not a steady rise.
    history[2].heartRate = 70;
    REQUIRE(engine.evaluateTrend(history).empty());
}

TEST_CASE("Rising systolic blood pressure") {
    AlertEngine engine;
    std::vector<VitalReading> history = {day(1, 70, 112, 7.5, 4), day(2, 70, 118, 7.5, 4),
                                         day(3, 70, 124, 7.5, 4), day(4, 70, 134, 7.5, 4)};
    auto alerts = engine.evaluateTrend(history);
    REQUIRE(alerts.size() == 1);
    REQUIRE(alerts[0].title == "Blood pressure trending up");
}

TEST_CASE("Low or falling mood") {
    AlertEngine engine;

    SECTION("persistently low mood") {
        std::vector<VitalReading> history = {day(1, 70, 115, 7.5, 4), day(2, 70, 115, 7.5, 2),
                                             day(3, 70, 115, 7.5, 2), day(4, 70, 115, 7.5, 1)};
        auto alerts = engine.evaluateTrend(history);
        REQUIRE(alerts.size() == 1);
        REQUIRE(alerts[0].title == "Ongoing low mood");
    }
    SECTION("mood falling but still above the low threshold") {
        std::vector<VitalReading> history = {day(1, 70, 115, 7.5, 5), day(2, 70, 115, 7.5, 5),
                                             day(3, 70, 115, 7.5, 4), day(4, 70, 115, 7.5, 3)};
        auto alerts = engine.evaluateTrend(history);
        REQUIRE(alerts.size() == 1);
        REQUIRE(alerts[0].title == "Mood trending down");
    }
}

TEST_CASE("History can be supplied in any order") {
    AlertEngine engine;
    std::vector<VitalReading> history = {day(4, 86, 115, 7.5, 4), day(1, 68, 115, 7.5, 4),
                                         day(3, 78, 115, 7.5, 4), day(2, 72, 115, 7.5, 4)};
    REQUIRE(hasTitle(engine.evaluateTrend(history), "Heart rate trending up"));
}

TEST_CASE("Readings with missing values are skipped in trends") {
    AlertEngine engine;
    // Day 3 has no sleep recorded; the three recorded nights are all short.
    std::vector<VitalReading> history = {day(1, 70, 115, 5.0, 4), day(2, 70, 115, 5.0, 4),
                                         day(3, 70, 115, 0.0, 4), day(4, 70, 115, 5.0, 4)};
    REQUIRE(hasTitle(engine.evaluateTrend(history), "Ongoing poor sleep"));
}

// ---------------------------------------------------------------------------
// Reference ranges and ranges.json
// ---------------------------------------------------------------------------

TEST_CASE("The shipped ranges.json matches the built-in defaults") {
    AlertEngine engine;
    std::string error;
    const std::string path = std::string(ASTRACARE_RESOURCE_DIR) + "/ranges.json";
    INFO(error);
    REQUIRE(engine.loadRangesFromFile(path, &error));

    const ReferenceRanges defaults = ReferenceRanges::defaults();
    REQUIRE(engine.ranges().heartRate == defaults.heartRate);
    REQUIRE(engine.ranges().systolic == defaults.systolic);
    REQUIRE(engine.ranges().diastolic == defaults.diastolic);
    REQUIRE(engine.ranges().sleepHours == defaults.sleepHours);
    REQUIRE(engine.ranges().mood == defaults.mood);
    REQUIRE(engine.ranges().trends == defaults.trends);
}

TEST_CASE("A partial ranges file overrides only what it contains") {
    TempFile file(R"({ "heartRate": { "caution": [60, 90] } })");
    AlertEngine engine;
    std::string error;
    REQUIRE(engine.loadRangesFromFile(file.path.string(), &error));

    // The new caution band applies...
    REQUIRE(engine.evaluate(withHeartRate(95))[0].level == AlertLevel::Caution);
    // ...the urgent band and the other vitals keep their defaults.
    REQUIRE(engine.ranges().heartRate.urgent == ReferenceRanges::defaults().heartRate.urgent);
    REQUIRE(engine.ranges().systolic == ReferenceRanges::defaults().systolic);
}

TEST_CASE("A custom engine can be built from ReferenceRanges directly") {
    ReferenceRanges ranges = ReferenceRanges::defaults();
    ranges.heartRate.caution = {55, 95};
    AlertEngine engine(ranges);
    REQUIRE(engine.evaluate(withHeartRate(96))[0].level == AlertLevel::Caution);
}

TEST_CASE("Bad ranges files are rejected and leave the ranges unchanged") {
    AlertEngine engine;
    const ReferenceRanges before = engine.ranges();
    std::string error;

    SECTION("missing file") {
        REQUIRE_FALSE(engine.loadRangesFromFile("/no/such/ranges.json", &error));
        REQUIRE_FALSE(error.empty());
    }
    SECTION("not JSON") {
        TempFile file("this is not json");
        REQUIRE_FALSE(engine.loadRangesFromFile(file.path.string(), &error));
        REQUIRE_FALSE(error.empty());
    }
    SECTION("band with the wrong shape") {
        TempFile file(R"({ "heartRate": { "caution": [60] } })");
        REQUIRE_FALSE(engine.loadRangesFromFile(file.path.string(), &error));
        REQUIRE_FALSE(error.empty());
    }
    SECTION("low greater than high") {
        TempFile file(R"({ "heartRate": { "caution": [100, 60] } })");
        REQUIRE_FALSE(engine.loadRangesFromFile(file.path.string(), &error));
    }
    SECTION("urgent band narrower than the caution band") {
        TempFile file(R"({ "heartRate": { "caution": [50, 100], "urgent": [60, 90] } })");
        REQUIRE_FALSE(engine.loadRangesFromFile(file.path.string(), &error));
    }
    SECTION("bad trend setting") {
        TempFile file(R"({ "trends": { "window": 1 } })");
        REQUIRE_FALSE(engine.loadRangesFromFile(file.path.string(), &error));
    }

    REQUIRE(engine.ranges().heartRate == before.heartRate);
    REQUIRE(engine.ranges().trends == before.trends);
}