#pragma once
// Owner: Role 6 (Storage, Build and Reports)

#include <string>
#include <vector>

#include "astracare/models.h"

namespace astracare {

struct MedicalReport {
    std::string generatedAt;
    std::vector<VitalReading> readings;
    std::vector<Alert> alerts;
    std::string notes;
};

class ReportBuilder {
public:
    // Plain-text/JSON form of a report. PDF export lives in the UI layer (Qt).
    static std::string toJson(const MedicalReport& report);
};

} // namespace astracare
