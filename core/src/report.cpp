#include "astracare/report.h"

#include <nlohmann/json.hpp>

#include <sstream>

namespace {

const char* levelName(astracare::AlertLevel level) {
    switch (level) {
    case astracare::AlertLevel::Normal: return "Normal";
    case astracare::AlertLevel::Caution: return "Caution";
    case astracare::AlertLevel::Urgent: return "Urgent";
    }
    return "Normal";
}

} // namespace

namespace astracare {

std::string ReportBuilder::toJson(const MedicalReport& report) {
    nlohmann::json payload;
    payload["generatedAt"] = report.generatedAt;
    payload["notes"] = report.notes;
    payload["guidanceIds"] = report.guidanceIds;
    payload["readings"] = nlohmann::json::array();
    for (const auto& reading : report.readings) {
        payload["readings"].push_back({
            {"id", reading.id}, {"timestamp", reading.timestamp},
            {"heartRate", reading.heartRate}, {"systolic", reading.systolic},
            {"diastolic", reading.diastolic}, {"sleepHours", reading.sleepHours},
            {"mood", reading.mood}, {"symptoms", reading.symptoms}
        });
    }
    payload["alerts"] = nlohmann::json::array();
    for (const auto& alert : report.alerts) {
        payload["alerts"].push_back({
            {"level", levelName(alert.level)}, {"title", alert.title},
            {"message", alert.message}, {"guidanceId", alert.guidanceId}
        });
    }
    return payload.dump(2);
}

std::string ReportBuilder::toCsv(const MedicalReport& report) {
    std::ostringstream csv;
    csv << "id,timestamp,heartRate,systolic,diastolic,sleepHours,mood,symptoms\n";
    for (const auto& reading : report.readings) {
        std::string symptoms = reading.symptoms;
        for (char& character : symptoms) {
            if (character == '"') character = '\'';
        }
        csv << reading.id << ",\"" << reading.timestamp << "\"," << reading.heartRate << ','
            << reading.systolic << ',' << reading.diastolic << ',' << reading.sleepHours << ','
            << reading.mood << ",\"" << symptoms << "\"\n";
    }
    return csv.str();
}

} // namespace astracare
