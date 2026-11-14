#pragma once
// Owner: Role 4 (Alerts Engineer)
//
// The alert engine turns vital readings into alerts. It is pure C++ (no Qt) and
// works fully offline: sensible default reference ranges are compiled in, and
// resources/ranges.json can override them.
//
// Astracare gives health guidance and alerts, NOT medical diagnoses. Every
// message produced here is worded as "may need attention", never as a diagnosis.

#include <optional>
#include <string>
#include <vector>

#include "astracare/models.h"

namespace astracare {

// Guidance ids that alerts point to (Alert::guidanceId). Role 5 must provide a
// guidance / emergency entry for each of these ids.
namespace guidance_ids {
inline constexpr const char* kHeartRate = "cardiac-001";
inline constexpr const char* kBloodPressure = "cardiac-002";
inline constexpr const char* kChestPain = "cardiac-003";
inline constexpr const char* kBreathing = "respiratory-001";
inline constexpr const char* kDizziness = "neuro-001";
inline constexpr const char* kNeurologicalEmergency = "neuro-002";
inline constexpr const char* kVision = "vision-001";
inline constexpr const char* kBleeding = "trauma-001";
inline constexpr const char* kFatigue = "general-001";
inline constexpr const char* kGeneralUnwell = "general-002";
inline constexpr const char* kSleep = "sleep-001";
inline constexpr const char* kMentalWellbeing = "mental-001";
} // namespace guidance_ids

// A range of values. An unset bound means "no limit on that side".
// contains(v) is true when low <= v <= high.
struct Band {
    std::optional<double> low;
    std::optional<double> high;

    bool contains(double v) const { return (!low || v >= *low) && (!high || v <= *high); }
    bool operator==(const Band& o) const { return low == o.low && high == o.high; }
    bool operator!=(const Band& o) const { return !(*this == o); }
};

// Thresholds for one vital sign.
//   value inside `caution`                -> Normal (no alert)
//   value outside `caution`, inside `urgent` -> Caution
//   value outside `urgent`                -> Urgent
// So `urgent` is the wider band and should contain `caution`.
struct VitalThresholds {
    std::string unit;
    Band caution;
    Band urgent;

    bool operator==(const VitalThresholds& o) const {
        return unit == o.unit && caution == o.caution && urgent == o.urgent;
    }
    bool operator!=(const VitalThresholds& o) const { return !(*this == o); }
};

// How many readings a pattern must span before a trend alert is raised.
struct TrendSettings {
    int sleepStreak = 3;    // this many poor-sleep readings in a row
    int moodStreak = 3;     // this many low-mood readings in a row
    int window = 4;         // readings examined for a steady rise / fall
    int heartRateRise = 15; // bpm increase across the window
    int systolicRise = 20;  // mmHg increase across the window
    int moodDrop = 2;       // mood points lost across the window

    bool operator==(const TrendSettings& o) const {
        return sleepStreak == o.sleepStreak && moodStreak == o.moodStreak && window == o.window &&
               heartRateRise == o.heartRateRise && systolicRise == o.systolicRise &&
               moodDrop == o.moodDrop;
    }
};

struct ReferenceRanges {
    VitalThresholds heartRate;
    VitalThresholds systolic;
    VitalThresholds diastolic;
    VitalThresholds sleepHours;
    VitalThresholds mood;
    TrendSettings trends;

    // The built-in ranges. Kept in sync with resources/ranges.json (a unit test
    // checks this), so the app still works if the file is missing.
    static ReferenceRanges defaults();
};

class AlertEngine {
  public:
    // Uses the built-in default ranges.
    AlertEngine();
    explicit AlertEngine(ReferenceRanges ranges);

    // Override the ranges from a JSON file (see resources/ranges.json). Anything
    // missing from the file keeps its current value. On failure returns false,
    // leaves the current ranges untouched and, if `error` is given, describes why.
    bool loadRangesFromFile(const std::string& path, std::string* error = nullptr);

    const ReferenceRanges& ranges() const { return ranges_; }

    // Evaluate one reading (vitals + symptoms). An empty vector means nothing
    // unusual was found. Alerts are sorted most severe first.
    //
    // A value of 0 (or a mood outside 1..5) means "not recorded" and is skipped,
    // so a partly filled-in reading never triggers false alarms.
    // Symptoms are matched case-insensitively as substrings, so "no chest pain"
    // still triggers: for a safety app a false alarm is better than a miss.
    std::vector<Alert> evaluate(const VitalReading& reading) const;

    // Look for patterns across several readings (poor sleep streak, rising heart
    // rate or blood pressure, low or falling mood). `history` can be in any order
    // and may include the latest reading; it is sorted by timestamp internally.
    std::vector<Alert> evaluateTrend(const std::vector<VitalReading>& history) const;

  private:
    ReferenceRanges ranges_;
};

// The most severe level in `alerts` (Normal if empty).
AlertLevel highestLevel(const std::vector<Alert>& alerts);

// "Normal", "Caution" or "Urgent" (for the UI and reports).
const char* toString(AlertLevel level);

} // namespace astracare