#pragma once
// Owner: Role 5 (Medical Content and Emergency)

#include <optional>
#include <string>
#include <vector>

#include "astracare/models.h"

namespace astracare {

class GuidanceLibrary {
public:
    // Load all guidance entries from a JSON file. Returns false on failure.
    bool loadFromFile(const std::string& path);

    // Load emergency procedures from the bundled emergency JSON schema.
    bool loadEmergencyFromFile(const std::string& path);

    std::optional<GuidanceEntry> findById(const std::string& id) const;
    std::vector<GuidanceEntry> search(const std::string& query) const;
    const std::vector<GuidanceEntry>& all() const { return entries_; }

private:
    std::vector<GuidanceEntry> entries_;
};

} // namespace astracare
