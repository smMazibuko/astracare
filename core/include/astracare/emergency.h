
#pragma once

#include <string>

namespace astracare {

enum class EmergencyType {
    SpacesuitPressureLeak,
    SuspectedCardiacArrest,
    SpacecraftDepressurization
};

struct EmergencyEvent {
    std::string emergencyId;
    std::string eventId;
    std::string astronautId;
    std::string location;
    std::string timestamp;
    EmergencyType type;
    std::string message;
};

class EmergencyService {
public:
    static EmergencyEvent createEvent(
        EmergencyType type,
        const std::string& astronautId,
        const std::string& location,
        const std::string& timestamp,
        const std::string& eventId
    );
};

} // namespace astracare
