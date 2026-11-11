#pragma once
// Owner: Role 4 (Alerts Engineer)

#include <vector>

#include "astracare/models.h"

namespace astracare {

class AlertEngine {
public:
    // Evaluate a single reading and return any alerts it triggers.
    // An empty vector means nothing unusual was found.
    std::vector<Alert> evaluate(const VitalReading& reading) const;

    // TODO(Role 4): evaluateTrend(const std::vector<VitalReading>& history)
    // TODO(Role 4): load reference ranges from resources/ranges.json
};

} // namespace astracare
