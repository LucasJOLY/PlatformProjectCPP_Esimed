#pragma once

#include <string>
#include <vector>
#include <optional>

namespace core {

    struct CustomLevel {
        int id;
        std::string name;
        std::string data;  // Level data in string format (like World::get_level_data)
    };

    class CustomLevelManager {
    public:
        static CustomLevelManager& instance();

        // CRUD operations
        void save_level(const CustomLevel& level);
        void delete_level(int id);
        std::optional<CustomLevel> get_level(int id) const;
        const std::vector<CustomLevel>& get_all_levels() const { return m_levels; }
        
        // Utility
        int get_next_id() const;
        void reload();

    private:
        CustomLevelManager();
        ~CustomLevelManager() = default;
        
        void load_from_file();
        void save_to_file();
        
        std::vector<CustomLevel> m_levels;
        static constexpr const char* SAVE_FILE = "custom_levels.json";
    };

} // namespace core
