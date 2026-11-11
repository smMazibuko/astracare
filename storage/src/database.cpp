#include "astracare/database.h"

namespace astracare {

bool Database::open(const std::string& /*path*/) {
    open_ = true;
    return true;
}

void Database::close() { open_ = false; }

int Database::insertReading(const VitalReading& reading) {
    if (!open_) return -1;
    VitalReading copy = reading;
    copy.id = nextId_++;
    stub_.push_back(copy);
    return copy.id;
}

std::vector<VitalReading> Database::allReadings() const { return stub_; }

} // namespace astracare
