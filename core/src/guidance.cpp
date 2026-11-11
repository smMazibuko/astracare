#include "astracare/guidance.h"

namespace astracare {

bool GuidanceLibrary::loadFromFile(const std::string& /*path*/) {
    // TODO(Role 5): parse resources/guidance/*.json (e.g. with nlohmann-json)
    return false;
}

std::optional<GuidanceEntry> GuidanceLibrary::findById(const std::string& id) const {
    for (const auto& e : entries_) {
        if (e.id == id) return e;
    }
    return std::nullopt;
}

std::vector<GuidanceEntry> GuidanceLibrary::search(const std::string& /*query*/) const {
    // TODO(Role 5): case-insensitive search over title, category and summary
    return {};
}

} // namespace astracare
