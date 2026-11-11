#include "astracare/report.h"

namespace astracare {

std::string ReportBuilder::toJson(const MedicalReport& report) {
    // TODO(Role 6): serialise readings and alerts properly
    return "{\"generatedAt\":\"" + report.generatedAt + "\"}";
}

} // namespace astracare
