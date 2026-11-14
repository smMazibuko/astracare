#include "astracare/alert_engine.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

namespace astracare {

namespace {

using Json = nlohmann::json;

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

std::string num(double v) {
    std::ostringstream out;
    out << v;
    return out.str();
}

std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

int rank(AlertLevel level) {
    return static_cast<int>(level);
}

// "50 to 100 bpm", "at least 3", "at most 10 hours"
std::string rangeText(const Band& band, const std::string& unit) {
    const std::string suffix = unit.empty() ? "" : " " + unit;
    if (band.low && band.high)
        return num(*band.low) + " to " + num(*band.high) + suffix;
    if (band.low)
        return "at least " + num(*band.low) + suffix;
    if (band.high)
        return "at most " + num(*band.high) + suffix;
    return "any value";
}

const char* adviceFor(AlertLevel level) {
    return level == AlertLevel::Urgent
               ? "This may need urgent attention. Open the linked procedure now."
               : "This may need attention. Review the linked guidance and re-check soon.";
}

// ---------------------------------------------------------------------------
// Single-reading vital checks
// ---------------------------------------------------------------------------

enum class Direction { InRange, Low, High };

struct Classification {
    AlertLevel level = AlertLevel::Normal;
    Direction direction = Direction::InRange;
};

Direction outside(const Band& band, double v) {
    if (band.low && v < *band.low)
        return Direction::Low;
    if (band.high && v > *band.high)
        return Direction::High;
    return Direction::InRange;
}

Classification classify(double v, const VitalThresholds& t) {
    if (Direction d = outside(t.urgent, v); d != Direction::InRange) {
        return {AlertLevel::Urgent, d};
    }
    if (Direction d = outside(t.caution, v); d != Direction::InRange) {
        return {AlertLevel::Caution, d};
    }
    return {};
}

struct VitalLabel {
    const char* name;      // used in sentences, e.g. "Heart rate"
    const char* lowTitle;  // e.g. "Low heart rate"
    const char* highTitle; // e.g. "High heart rate"
    const char* guidanceId;
};

void addVitalAlert(std::vector<Alert>& out, double value, const VitalThresholds& t,
                   const VitalLabel& label) {
    const Classification c = classify(value, t);
    if (c.level == AlertLevel::Normal)
        return;

    const bool high = c.direction == Direction::High;
    Alert alert;
    alert.level = c.level;
    alert.title = high ? label.highTitle : label.lowTitle;
    alert.message = std::string(label.name) + " is " + num(value) + " " + t.unit + ", " +
                    (high ? "above" : "below") + " the expected range of " +
                    rangeText(t.caution, t.unit) + ". " + adviceFor(c.level);
    alert.guidanceId = label.guidanceId;
    out.push_back(std::move(alert));
}

// Systolic and diastolic are reported together as one blood pressure alert.
void addBloodPressureAlert(std::vector<Alert>& out, const VitalReading& r,
                           const ReferenceRanges& ranges) {
    const bool haveSys = r.systolic > 0;
    const bool haveDia = r.diastolic > 0;
    if (!haveSys && !haveDia)
        return;

    const Classification sys = haveSys ? classify(r.systolic, ranges.systolic) : Classification{};
    const Classification dia = haveDia ? classify(r.diastolic, ranges.diastolic) : Classification{};
    const Classification& worst = rank(sys.level) >= rank(dia.level) ? sys : dia;
    if (worst.level == AlertLevel::Normal)
        return;

    std::string reading;
    if (haveSys && haveDia) {
        reading = "Blood pressure is " + std::to_string(r.systolic) + "/" +
                  std::to_string(r.diastolic) + " mmHg";
    } else if (haveSys) {
        reading = "Systolic blood pressure is " + std::to_string(r.systolic) + " mmHg";
    } else {
        reading = "Diastolic blood pressure is " + std::to_string(r.diastolic) + " mmHg";
    }

    const bool high = worst.direction == Direction::High;
    Alert alert;
    alert.level = worst.level;
    alert.title = high ? "High blood pressure" : "Low blood pressure";
    alert.message = reading + ", " + (high ? "above" : "below") + " the expected range (systolic " +
                    rangeText(ranges.systolic.caution, "") + ", diastolic " +
                    rangeText(ranges.diastolic.caution, "") + " mmHg). " + adviceFor(worst.level);
    alert.guidanceId = guidance_ids::kBloodPressure;
    out.push_back(std::move(alert));
}

// ---------------------------------------------------------------------------
// Symptom rules
// ---------------------------------------------------------------------------

struct SymptomRule {
    AlertLevel level;
    const char* title;
    const char* message;
    const char* guidanceId;
    std::vector<const char*> keywords; // lowercase, matched as substrings
};

const std::vector<SymptomRule>& symptomRules() {
    using namespace guidance_ids;
    static const std::vector<SymptomRule> rules = {
        // ---- Urgent ----
        {AlertLevel::Urgent,
         "Chest pain reported",
         "Chest pain or pressure was reported. This may need urgent attention. Open the "
         "emergency procedure now.",
         kChestPain,
         {"chest pain", "chest pressure", "chest tightness", "chest discomfort"}},
        {AlertLevel::Urgent,
         "Difficulty breathing reported",
         "Difficulty breathing was reported. This may need urgent attention. Open the "
         "emergency procedure now.",
         kBreathing,
         {"difficulty breathing", "trouble breathing", "short of breath", "shortness of breath",
          "breathless", "can't breathe", "cant breathe", "cannot breathe"}},
        {AlertLevel::Urgent,
         "Loss of consciousness reported",
         "Fainting or loss of consciousness was reported. This may need urgent attention. Open "
         "the emergency procedure now.",
         kNeurologicalEmergency,
         {"faint", "passed out", "blacked out", "unconscious", "loss of consciousness"}},
        {AlertLevel::Urgent,
         "Neurological warning sign reported",
         "Confusion, seizure or slurred speech was reported. This may need urgent attention. "
         "Open the emergency procedure now.",
         kNeurologicalEmergency,
         {"confus", "seizure", "slurred speech"}},
        {AlertLevel::Urgent,
         "Serious bleeding reported",
         "Serious bleeding was reported. This may need urgent attention. Open the emergency "
         "procedure now.",
         kBleeding,
         {"severe bleeding", "heavy bleeding", "vomiting blood", "coughing blood"}},

        // ---- Caution ----
        {AlertLevel::Caution,
         "Dizziness reported",
         "Dizziness was reported. This may need attention. Review the linked guidance and "
         "re-check soon.",
         kDizziness,
         {"dizz", "lightheaded", "light-headed", "light headed", "vertigo"}},
        {AlertLevel::Caution,
         "Heart palpitations reported",
         "Palpitations or an irregular heartbeat were reported. This may need attention. Review "
         "the linked guidance and re-check soon.",
         kHeartRate,
         {"palpitation", "racing heart", "irregular heartbeat", "skipped beat", "heart flutter"}},
        {AlertLevel::Caution,
         "Vision changes reported",
         "Vision changes were reported. This may need attention. Review the linked guidance and "
         "re-check soon.",
         kVision,
         {"vision", "blurry", "blurred"}},
        {AlertLevel::Caution,
         "Tiredness reported",
         "Tiredness or weakness was reported. This may need attention. Review the linked "
         "guidance and re-check soon.",
         kFatigue,
         {"tired", "fatigue", "exhaust", "weak", "drowsy"}},
        {AlertLevel::Caution,
         "Feeling unwell",
         "Headache, nausea or fever was reported. This may need attention. Review the linked "
         "guidance and re-check soon.",
         kGeneralUnwell,
         {"headache", "nausea", "nauseous", "vomit", "fever", "chills"}},
        {AlertLevel::Caution,
         "Stress or low mood reported",
         "Stress, anxiety or low mood was reported. This may need attention. Review the linked "
         "wellbeing guidance.",
         kMentalWellbeing,
         {"anxious", "anxiety", "panic", "depress", "hopeless", "stressed", "overwhelmed",
          "lonely"}},
    };
    return rules;
}

void addSymptomAlerts(std::vector<Alert>& out, const std::string& symptoms) {
    if (symptoms.empty())
        return;
    const std::string text = lower(symptoms);
    for (const SymptomRule& rule : symptomRules()) {
        const bool hit =
            std::any_of(rule.keywords.begin(), rule.keywords.end(),
                        [&](const char* k) { return text.find(k) != std::string::npos; });
        if (hit)
            out.push_back({rule.level, rule.title, rule.message, rule.guidanceId});
    }
}

// ---------------------------------------------------------------------------
// Trend helpers
// ---------------------------------------------------------------------------

template <class Extract>
std::vector<double> series(const std::vector<VitalReading>& history, Extract extract) {
    std::vector<double> values;
    for (const VitalReading& r : history) {
        if (std::optional<double> v = extract(r))
            values.push_back(*v);
    }
    return values;
}

// True if the last `n` values all satisfy `pred`.
template <class Pred> bool lastAll(const std::vector<double>& v, int n, Pred pred) {
    if (n < 1 || static_cast<int>(v.size()) < n)
        return false;
    return std::all_of(v.end() - n, v.end(), pred);
}

// True if, over the last `window` values, the series never moves the wrong way
// and has moved by at least `minChange` in total. rising=true looks for an
// increase, rising=false for a decrease.
bool steadyChange(const std::vector<double>& v, int window, double minChange, bool rising) {
    if (window < 2 || static_cast<int>(v.size()) < window)
        return false;
    const auto first = v.end() - window;
    for (auto it = first + 1; it != v.end(); ++it) {
        const double step = *it - *(it - 1);
        if (rising ? step < 0 : step > 0)
            return false;
    }
    const double total = rising ? v.back() - *first : *first - v.back();
    return total >= minChange;
}

std::optional<double> positive(double v) {
    if (v > 0)
        return v;
    return std::nullopt;
}

// ---------------------------------------------------------------------------
// Loading ranges from JSON
// ---------------------------------------------------------------------------

bool parseBound(const Json& node, std::optional<double>& out) {
    if (node.is_null()) {
        out = std::nullopt;
        return true;
    }
    if (node.is_number()) {
        out = node.get<double>();
        return true;
    }
    return false;
}

bool parseBand(const Json& node, const std::string& where, Band& band, std::string& problem) {
    if (!node.is_array() || node.size() != 2) {
        problem = where + " must be a two-element array [low, high]";
        return false;
    }
    Band parsed;
    if (!parseBound(node[0], parsed.low) || !parseBound(node[1], parsed.high)) {
        problem = where + " bounds must be numbers or null";
        return false;
    }
    if (parsed.low && parsed.high && *parsed.low > *parsed.high) {
        problem = where + " has low greater than high";
        return false;
    }
    band = parsed;
    return true;
}

bool parseVital(const Json& doc, const std::string& key, VitalThresholds& t, std::string& problem) {
    if (!doc.contains(key))
        return true; // keep the current value
    const Json& node = doc[key];
    if (!node.is_object()) {
        problem = key + " must be an object";
        return false;
    }
    VitalThresholds next = t;
    if (node.contains("unit")) {
        if (!node["unit"].is_string()) {
            problem = key + ".unit must be a string";
            return false;
        }
        next.unit = node["unit"].get<std::string>();
    }
    if (node.contains("caution") &&
        !parseBand(node["caution"], key + ".caution", next.caution, problem)) {
        return false;
    }
    if (node.contains("urgent") &&
        !parseBand(node["urgent"], key + ".urgent", next.urgent, problem)) {
        return false;
    }
    // The urgent band must not be narrower than the caution band.
    const bool lowOk =
        !next.caution.low || !next.urgent.low || *next.urgent.low <= *next.caution.low;
    const bool highOk =
        !next.caution.high || !next.urgent.high || *next.urgent.high >= *next.caution.high;
    if (!lowOk || !highOk) {
        problem = key + ": the urgent range must contain the caution range";
        return false;
    }
    t = next;
    return true;
}

bool parseCount(const Json& node, const std::string& key, int minimum, int& out,
                std::string& problem) {
    if (!node.contains(key))
        return true;
    if (!node[key].is_number_integer() || node[key].get<int>() < minimum) {
        problem = "trends." + key + " must be a whole number >= " + std::to_string(minimum);
        return false;
    }
    out = node[key].get<int>();
    return true;
}

bool parseTrends(const Json& doc, TrendSettings& trends, std::string& problem) {
    if (!doc.contains("trends"))
        return true;
    const Json& node = doc["trends"];
    if (!node.is_object()) {
        problem = "trends must be an object";
        return false;
    }
    TrendSettings next = trends;
    if (!parseCount(node, "sleepStreak", 1, next.sleepStreak, problem) ||
        !parseCount(node, "moodStreak", 1, next.moodStreak, problem) ||
        !parseCount(node, "window", 2, next.window, problem) ||
        !parseCount(node, "heartRateRise", 1, next.heartRateRise, problem) ||
        !parseCount(node, "systolicRise", 1, next.systolicRise, problem) ||
        !parseCount(node, "moodDrop", 1, next.moodDrop, problem)) {
        return false;
    }
    trends = next;
    return true;
}

} // namespace

// ---------------------------------------------------------------------------
// ReferenceRanges
// ---------------------------------------------------------------------------

// NOTE: these values are reasonable adult defaults for the hackathon prototype.
// They are NOT a clinical standard and must be reviewed by qualified medical
// professionals before real use (see the README roadmap).
ReferenceRanges ReferenceRanges::defaults() {
    ReferenceRanges r;
    r.heartRate = {"bpm", {50, 100}, {40, 130}};
    r.systolic = {"mmHg", {90, 139}, {80, 180}};
    r.diastolic = {"mmHg", {60, 89}, {50, 120}};
    r.sleepHours = {"hours", {6, 10}, {4, std::nullopt}};
    r.mood = {"", {3, std::nullopt}, {std::nullopt, std::nullopt}};
    return r;
}

// ---------------------------------------------------------------------------
// AlertEngine
// ---------------------------------------------------------------------------

AlertEngine::AlertEngine() : ranges_(ReferenceRanges::defaults()) {}

AlertEngine::AlertEngine(ReferenceRanges ranges) : ranges_(std::move(ranges)) {}

bool AlertEngine::loadRangesFromFile(const std::string& path, std::string* error) {
    std::string problem;
    auto fail = [&](const std::string& message) {
        if (error)
            *error = message;
        return false;
    };

    std::ifstream in(path);
    if (!in)
        return fail("cannot open " + path);

    const Json doc = Json::parse(in, nullptr, /*allow_exceptions=*/false);
    if (doc.is_discarded() || !doc.is_object())
        return fail("invalid JSON in " + path);

    ReferenceRanges next = ranges_;
    if (!parseVital(doc, "heartRate", next.heartRate, problem) ||
        !parseVital(doc, "systolic", next.systolic, problem) ||
        !parseVital(doc, "diastolic", next.diastolic, problem) ||
        !parseVital(doc, "sleepHours", next.sleepHours, problem) ||
        !parseVital(doc, "mood", next.mood, problem) || !parseTrends(doc, next.trends, problem)) {
        return fail(problem + " (" + path + ")");
    }

    ranges_ = std::move(next);
    return true;
}

std::vector<Alert> AlertEngine::evaluate(const VitalReading& reading) const {
    std::vector<Alert> alerts;

    if (reading.heartRate > 0) {
        addVitalAlert(
            alerts, reading.heartRate, ranges_.heartRate,
            {"Heart rate", "Low heart rate", "High heart rate", guidance_ids::kHeartRate});
    }
    addBloodPressureAlert(alerts, reading, ranges_);
    if (reading.sleepHours > 0) {
        addVitalAlert(alerts, reading.sleepHours, ranges_.sleepHours,
                      {"Sleep", "Short sleep", "Long sleep", guidance_ids::kSleep});
    }
    if (reading.mood >= 1 && reading.mood <= 5) {
        addVitalAlert(alerts, reading.mood, ranges_.mood,
                      {"Mood score", "Low mood", "High mood", guidance_ids::kMentalWellbeing});
    }
    addSymptomAlerts(alerts, reading.symptoms);

    std::stable_sort(alerts.begin(), alerts.end(),
                     [](const Alert& a, const Alert& b) { return rank(a.level) > rank(b.level); });
    return alerts;
}

std::vector<Alert> AlertEngine::evaluateTrend(const std::vector<VitalReading>& history) const {
    std::vector<Alert> alerts;
    if (history.size() < 2)
        return alerts;

    // ISO 8601 timestamps sort correctly as plain strings.
    std::vector<VitalReading> sorted = history;
    std::stable_sort(
        sorted.begin(), sorted.end(),
        [](const VitalReading& a, const VitalReading& b) { return a.timestamp < b.timestamp; });

    const TrendSettings& t = ranges_.trends;

    // Poor sleep several readings in a row.
    if (ranges_.sleepHours.caution.low) {
        const double limit = *ranges_.sleepHours.caution.low;
        const auto sleep =
            series(sorted, [](const VitalReading& r) { return positive(r.sleepHours); });
        if (lastAll(sleep, t.sleepStreak, [limit](double h) { return h < limit; })) {
            alerts.push_back({AlertLevel::Caution, "Ongoing poor sleep",
                              "Sleep has been under " + num(limit) + " hours for the last " +
                                  std::to_string(t.sleepStreak) +
                                  " readings. This may need attention. Review the sleep guidance.",
                              guidance_ids::kSleep});
        }
    }

    // Heart rate creeping up.
    const auto heartRate =
        series(sorted, [](const VitalReading& r) { return positive(r.heartRate); });
    if (steadyChange(heartRate, t.window, t.heartRateRise, /*rising=*/true)) {
        alerts.push_back({AlertLevel::Caution, "Heart rate trending up",
                          "Heart rate rose from " + num(*(heartRate.end() - t.window)) + " to " +
                              num(heartRate.back()) + " bpm over the last " +
                              std::to_string(t.window) +
                              " readings. This may need attention. Review the linked guidance.",
                          guidance_ids::kHeartRate});
    }

    // Systolic blood pressure creeping up.
    const auto systolic =
        series(sorted, [](const VitalReading& r) { return positive(r.systolic); });
    if (steadyChange(systolic, t.window, t.systolicRise, /*rising=*/true)) {
        alerts.push_back({AlertLevel::Caution, "Blood pressure trending up",
                          "Systolic pressure rose from " + num(*(systolic.end() - t.window)) +
                              " to " + num(systolic.back()) + " mmHg over the last " +
                              std::to_string(t.window) +
                              " readings. This may need attention. Review the linked guidance.",
                          guidance_ids::kBloodPressure});
    }

    // Mood: persistently low, otherwise steadily falling.
    const auto mood = series(sorted, [](const VitalReading& r) -> std::optional<double> {
        if (r.mood >= 1 && r.mood <= 5)
            return r.mood;
        return std::nullopt;
    });
    bool lowMood = false;
    if (ranges_.mood.caution.low) {
        const double limit = *ranges_.mood.caution.low;
        lowMood = lastAll(mood, t.moodStreak, [limit](double m) { return m < limit; });
    }
    if (lowMood) {
        alerts.push_back({AlertLevel::Caution, "Ongoing low mood",
                          "Mood has been low for the last " + std::to_string(t.moodStreak) +
                              " readings. This may need attention. Review the wellbeing guidance.",
                          guidance_ids::kMentalWellbeing});
    } else if (steadyChange(mood, t.window, t.moodDrop, /*rising=*/false)) {
        alerts.push_back({AlertLevel::Caution, "Mood trending down",
                          "Mood has fallen from " + num(*(mood.end() - t.window)) + " to " +
                              num(mood.back()) + " over the last " + std::to_string(t.window) +
                              " readings. This may need attention. Review the wellbeing guidance.",
                          guidance_ids::kMentalWellbeing});
    }

    return alerts;
}

// ---------------------------------------------------------------------------
// Free functions
// ---------------------------------------------------------------------------

AlertLevel highestLevel(const std::vector<Alert>& alerts) {
    AlertLevel highest = AlertLevel::Normal;
    for (const Alert& a : alerts) {
        if (rank(a.level) > rank(highest))
            highest = a.level;
    }
    return highest;
}

const char* toString(AlertLevel level) {
    switch (level) {
    case AlertLevel::Normal:
        return "Normal";
    case AlertLevel::Caution:
        return "Caution";
    case AlertLevel::Urgent:
        return "Urgent";
    }
    return "Normal";
}

} // namespace astracare