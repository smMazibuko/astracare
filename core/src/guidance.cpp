
#include "astracare/guidance.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace astracare {

// Load NASA medical guidance from a JSON file
bool GuidanceLibrary::loadFromFile(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        return false;
    }

    try {
        nlohmann::json data;
        file >> data;

        // Validate the JSON structure
        if (!data.is_object() ||
            !data.contains("entries") ||
            !data["entries"].is_array()) {
            return false;
        }

        std::vector<GuidanceEntry> loaded;

        // Read each medical guide
        for (const auto& item : data["entries"]) {
            GuidanceEntry entry;

            entry.id = item.at("id").get<std::string>();
            entry.title = item.at("title").get<std::string>();
            entry.category = item.at("category").get<std::string>();
            entry.summary = item.at("summary").get<std::string>();
            entry.source = item.at("source").get<std::string>();

            // Convert JSON steps array to a numbered string
            const auto& steps = item.at("steps");

            if (!steps.is_array()) {
                return false;
            }

            int number = 1;

            for (const auto& step : steps) {
                entry.steps +=
                    std::to_string(number++) + ". " +
                    step.get<std::string>() + "\n";
            }

            loaded.push_back(std::move(entry));
        }

        // Save successfully loaded guides
        entries_ = std::move(loaded);

        return true;

    } catch (const nlohmann::json::exception&) {
        return false;
    }
}

// Find a medical guide using its unique ID
std::optional<GuidanceEntry> GuidanceLibrary::findById(
    const std::string& id) const {

    for (const auto& entry : entries_) {
        if (entry.id == id) {
            return entry;
        }
    }

    return std::nullopt;
}

// Search medical guides by title, category or summary
std::vector<GuidanceEntry> GuidanceLibrary::search(
    const std::string& query) const {

    std::vector<GuidanceEntry> results;

    // Convert text to lowercase for case-insensitive searching
    auto toLower = [](std::string text) {
        std::transform(
            text.begin(),
            text.end(),
            text.begin(),
            [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            }
        );
        return text;
    };

    const std::string searchQuery = toLower(query);

    for (const auto& entry : entries_) {
        const std::string title = toLower(entry.title);
        const std::string category = toLower(entry.category);
        const std::string summary = toLower(entry.summary);

        if (title.find(searchQuery) != std::string::npos ||
            category.find(searchQuery) != std::string::npos ||
            summary.find(searchQuery) != std::string::npos) {

            results.push_back(entry);
        }
    }

    return results;
}

} // namespace astracare
