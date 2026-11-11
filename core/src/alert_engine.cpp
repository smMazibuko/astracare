#include "astracare/alert_engine.h"

namespace astracare {

std::vector<Alert> AlertEngine::evaluate(const VitalReading& reading) const {
    std::vector<Alert> alerts;

    // PLACEHOLDER rule so the pipeline can be tested end to end.
    // These numbers are NOT medical guidance. Role 4 replaces them with
    // reviewed reference ranges.
    if (reading.heartRate > 120) {
        alerts.push_back({AlertLevel::Urgent,
                          "High heart rate",
                          "Heart rate is above the placeholder threshold.",
                          "cardiac-001"});
    }

    return alerts;
}

} // namespace astracare
