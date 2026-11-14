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
    std::vector<std::string> guidanceIds;
    std::string notes;
};

class ReportBuilder {
public:
    // Portable report formats. PDF export lives in the UI layer (Qt).
    static std::string toJson(const MedicalReport& report);
    static std::string toCsv(const MedicalReport& report);
};

} // namespace astracare
