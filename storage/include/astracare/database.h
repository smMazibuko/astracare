#pragma once
// Owner: Role 6 (Storage, Build and Reports)

#include <string>
#include <vector>

#include "astracare/models.h"

struct sqlite3;

namespace astracare {

class Database {
public:
    Database() = default;
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Open (or create) the database file. Use ":memory:" for tests.
    bool open(const std::string& path);
    void close();

    // Returns the new row id, or -1 on failure.
    int insertReading(const VitalReading& reading);
    std::vector<VitalReading> allReadings() const;

private:
    ::sqlite3* handle_ = nullptr;
};

} // namespace astracare
