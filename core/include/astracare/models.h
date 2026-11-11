#pragma once
// Shared data models. Changes here affect everyone: tell the team first.

#include <string>

namespace astracare {

struct VitalReading {
    int id = 0;
    std::string timestamp;   // ISO 8601, e.g. "2026-11-11T09:30:00Z"
    int heartRate = 0;       // bpm
    int systolic = 0;        // mmHg
    int diastolic = 0;       // mmHg
    double sleepHours = 0.0; // hours slept
    int mood = 3;            // 1 (low) to 5 (high)
    std::string symptoms;    // free text or comma-separated tags
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
