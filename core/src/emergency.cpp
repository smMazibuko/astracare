
#include "astracare/emergency.h"

namespace astracare {

EmergencyEvent EmergencyService::createEvent(
    EmergencyType type,
    const std::string& astronautId,
    const std::string& location,
    const std::string& timestamp,
    const std::string& eventId
) {
    EmergencyEvent event;

    event.eventId = eventId;
    event.astronautId = astronautId;
    event.location = location;
    event.timestamp = timestamp;
    event.type = type;

    switch (type) {
        case EmergencyType::SpacesuitPressureLeak:
            event.emergencyId = "emergency-eva-001";
            event.message =
                "CRITICAL: Spacesuit pressure leak detected. "
                "Notify nearby crew and initiate the applicable "
                "EVA contingency procedure.";
            break;

        case EmergencyType::SuspectedCardiacArrest:
            event.emergencyId = "emergency-cardiac-001";
            event.message =
                "CRITICAL: Suspected cardiac arrest. "
                "Notify the Crew Medical Officer and nearby crew.";
            break;

        case EmergencyType::SpacecraftDepressurization:
            event.emergencyId = "emergency-depressurization-001";
            event.message =
                "CRITICAL: Spacecraft depressurization warning. "
                "Notify crew and follow the approved spacecraft "
                "emergency procedure.";
            break;
    }

    return event;
}

} // namespace astracare
