#include "astracare/database.h"

#include <sqlite3.h>

#include <utility>

namespace astracare {

namespace {

bool execute(sqlite3* db, const char* sql) {
    char* error = nullptr;
    const int result = sqlite3_exec(db, sql, nullptr, nullptr, &error);
    sqlite3_free(error);
    return result == SQLITE_OK;
}

} // namespace

Database::~Database() { close(); }

bool Database::open(const std::string& path) {
    close();
    sqlite3* opened = nullptr;
    if (sqlite3_open(path.c_str(), &opened) != SQLITE_OK) {
        if (opened != nullptr) sqlite3_close(opened);
        return false;
    }

    constexpr const char* schema = R"sql(
        CREATE TABLE IF NOT EXISTS vital_readings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT NOT NULL,
            heart_rate INTEGER NOT NULL,
            systolic INTEGER NOT NULL,
            diastolic INTEGER NOT NULL,
            sleep_hours REAL NOT NULL,
            mood INTEGER NOT NULL,
            symptoms TEXT NOT NULL DEFAULT ''
        );
        CREATE INDEX IF NOT EXISTS idx_vital_readings_timestamp
            ON vital_readings(timestamp);
    )sql";
    if (!execute(opened, schema)) {
        sqlite3_close(opened);
        return false;
    }
    handle_ = opened;
    return true;
}

void Database::close() {
    if (handle_ != nullptr) {
        sqlite3_close(handle_);
        handle_ = nullptr;
    }
}

int Database::insertReading(const VitalReading& reading) {
    if (handle_ == nullptr) return -1;
    constexpr const char* sql = R"sql(
        INSERT INTO vital_readings
            (timestamp, heart_rate, systolic, diastolic, sleep_hours, mood, symptoms)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )sql";
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(handle_, sql, -1, &statement, nullptr) != SQLITE_OK)
        return -1;
    sqlite3_bind_text(statement, 1, reading.timestamp.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 2, reading.heartRate);
    sqlite3_bind_int(statement, 3, reading.systolic);
    sqlite3_bind_int(statement, 4, reading.diastolic);
    sqlite3_bind_double(statement, 5, reading.sleepHours);
    sqlite3_bind_int(statement, 6, reading.mood);
    sqlite3_bind_text(statement, 7, reading.symptoms.c_str(), -1, SQLITE_TRANSIENT);
    const int result = sqlite3_step(statement);
    sqlite3_finalize(statement);
    return result == SQLITE_DONE ? static_cast<int>(sqlite3_last_insert_rowid(handle_)) : -1;
}

std::vector<VitalReading> Database::allReadings() const {
    std::vector<VitalReading> readings;
    if (handle_ == nullptr) return readings;
    constexpr const char* sql = R"sql(
        SELECT id, timestamp, heart_rate, systolic, diastolic, sleep_hours, mood, symptoms
        FROM vital_readings ORDER BY timestamp ASC, id ASC
    )sql";
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(handle_, sql, -1, &statement, nullptr) != SQLITE_OK)
        return readings;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        VitalReading reading{};
        reading.id = sqlite3_column_int(statement, 0);
        reading.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
        reading.heartRate = sqlite3_column_int(statement, 2);
        reading.systolic = sqlite3_column_int(statement, 3);
        reading.diastolic = sqlite3_column_int(statement, 4);
        reading.sleepHours = sqlite3_column_double(statement, 5);
        reading.mood = sqlite3_column_int(statement, 6);
        reading.symptoms = reinterpret_cast<const char*>(sqlite3_column_text(statement, 7));
        readings.push_back(std::move(reading));
    }
    sqlite3_finalize(statement);
    return readings;
}

} // namespace astracare
