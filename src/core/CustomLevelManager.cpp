#include "CustomLevelManager.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace core {

    CustomLevelManager& CustomLevelManager::instance() {
        static CustomLevelManager instance;
        return instance;
    }

    CustomLevelManager::CustomLevelManager() {
        load_from_file();
    }

    void CustomLevelManager::load_from_file() {
        m_levels.clear();
        
        std::ifstream file(SAVE_FILE);
        if (!file.is_open()) {
            std::cout << "No custom levels file found, starting fresh." << std::endl;
            return;
        }

        std::string line;
        CustomLevel current;
        bool in_level = false;
        std::string data_buffer;

        while (std::getline(file, line)) {
            // Simple text-based parsing
            if (line.find("LEVEL_START:") == 0) {
                in_level = true;
                current = CustomLevel{};
                current.id = std::stoi(line.substr(12));
                data_buffer.clear();
            } else if (line.find("NAME:") == 0 && in_level) {
                current.name = line.substr(5);
            } else if (line.find("DATA_START") == 0 && in_level) {
                // Data follows on next lines
            } else if (line.find("DATA_END") == 0 && in_level) {
                current.data = data_buffer;
            } else if (line.find("LEVEL_END") == 0 && in_level) {
                m_levels.push_back(current);
                in_level = false;
            } else if (in_level && line.find("NAME:") != 0 && line.find("DATA_START") != 0) {
                // Part of level data
                if (!data_buffer.empty()) data_buffer += "\n";
                data_buffer += line;
            }
        }

        std::cout << "Loaded " << m_levels.size() << " custom levels." << std::endl;
    }

    void CustomLevelManager::save_to_file() {
        std::ofstream file(SAVE_FILE);
        if (!file.is_open()) {
            std::cerr << "Failed to save custom levels!" << std::endl;
            return;
        }

        for (const auto& level : m_levels) {
            file << "LEVEL_START:" << level.id << "\n";
            file << "NAME:" << level.name << "\n";
            file << "DATA_START\n";
            file << level.data << "\n";
            file << "DATA_END\n";
            file << "LEVEL_END\n";
        }

        std::cout << "Saved " << m_levels.size() << " custom levels." << std::endl;
    }

    void CustomLevelManager::save_level(const CustomLevel& level) {
        // Update existing or add new
        auto it = std::find_if(m_levels.begin(), m_levels.end(),
            [&level](const CustomLevel& l) { return l.id == level.id; });
        
        if (it != m_levels.end()) {
            *it = level;
        } else {
            m_levels.push_back(level);
        }
        
        save_to_file();
    }

    void CustomLevelManager::delete_level(int id) {
        m_levels.erase(
            std::remove_if(m_levels.begin(), m_levels.end(),
                [id](const CustomLevel& l) { return l.id == id; }),
            m_levels.end()
        );
        save_to_file();
    }

    std::optional<CustomLevel> CustomLevelManager::get_level(int id) const {
        auto it = std::find_if(m_levels.begin(), m_levels.end(),
            [id](const CustomLevel& l) { return l.id == id; });
        
        if (it != m_levels.end()) {
            return *it;
        }
        return std::nullopt;
    }

    int CustomLevelManager::get_next_id() const {
        int max_id = 0;
        for (const auto& level : m_levels) {
            max_id = std::max(max_id, level.id);
        }
        return max_id + 1;
    }

    void CustomLevelManager::reload() {
        load_from_file();
    }

} // namespace core
