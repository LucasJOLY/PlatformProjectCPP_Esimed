#pragma once

#include <string>
#include <fstream>
#include <map>
#include <vector>

namespace core {

    class LevelProgress {
    public:
        static LevelProgress& instance();
        
        // Get stars for a level (0-3)
        int get_stars(int level_id) const;
        
        // Set stars for a level
        void set_stars(int level_id, int stars);
        
        // Check if level is unlocked
        bool is_unlocked(int level_id) const;
        
        // Calculate stars based on coins and lives
        int calculate_stars(int coins_collected, int total_coins, int lives_remaining) const;
        
        // Wallet
        int get_wallet() const { return m_total_coins_wallet; }
        void add_coins(int amount);
        bool spend_coins(int amount);
        
        // Skins
        bool is_skin_unlocked(const std::string& skin_name) const;
        void unlock_skin(const std::string& skin_name);
        std::string get_selected_skin() const { return m_selected_skin; }
        void select_skin(const std::string& skin_name);
        
        // Save/load progress
        void save();
        void load();
        
    private:
        LevelProgress();
        ~LevelProgress() = default;
        
        std::map<int, int> m_level_stars; // level_id -> stars (0-3)
        int m_total_coins_wallet;
        std::string m_selected_skin;
        std::vector<std::string> m_unlocked_skins;
        
        std::string m_save_file;
    };

} // namespace core
