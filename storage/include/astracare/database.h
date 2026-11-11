#pragma once
// Owner: Role 6 (Storage, Build and Reports)

#include <string>
#include <vector>

#include "astracare/models.h"

namespace astracare {

class Database {
public:
    // Open (or create) the database file. Use ":memory:" for tests.
    bool open(const std::string& path);
    void close();

    // Returns the new row id, or -1 on failure.
    int insertReading(const VitalReading& reading);
    std::vector<VitalReading> allReadings() const;

private:
    // TODO(Role 6): replace with a real SQLite handle and schema.
    // This in-memory stub lets everyone code against the API from day one.
    std::vector<VitalReading> stub_;
    int nextId_ = 1;
    bool open_ = false;
};

} // namespace astracare
