#pragma once
// Shared data models. Changes here affect everyone: tell the team first.

#include <string>

namespace astracare {

struct VitalReading {
    int id = 0;
    std::string timestamp; // ISO 8601
    int heartRate = 0;     // bpm
    int systolic = 0;      // mmHg
    int diastolic = 0;     // mmHg
    double sleepHours = 0;
    int mood = 0;          // 1 to 5
    std::string symptoms; // free text
};

enum class AlertLevel { Normal, Caution, Urgent };

struct Alert {
    AlertLevel level = AlertLevel::Normal;
    std::string title;
    std::string message;
    std::string guidanceId; // links to a GuidanceEntry id
};

struct GuidanceEntry {
    std::string id;
    std::string title;
    std::string category; // e.g. "cardiac", "respiratory", "mental-health"
    std::string summary;
    std::string steps;    // TODO(Role 5): consider std::vector<std::string>
    std::string source;   // where the content came from
};

} // namespace astracare
