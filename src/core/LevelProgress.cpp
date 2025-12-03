#include "LevelProgress.hpp"
#include <iostream>
#include <filesystem>

namespace core {

    LevelProgress& LevelProgress::instance() {
        static LevelProgress instance;
        return instance;
    }
    
    LevelProgress::LevelProgress() : m_total_coins_wallet(0), m_selected_skin("beige") {
        // Default skin is unlocked
        m_unlocked_skins.push_back("beige");
        
        // Save file in user's home directory
        std::string home = std::getenv("HOME") ? std::getenv("HOME") : ".";
        m_save_file = home + "/.cosmic_quest_progress.dat";
        load();
    }
    
    int LevelProgress::get_stars(int level_id) const {
        if (m_level_stars.find(level_id) != m_level_stars.end()) {
            return m_level_stars.at(level_id);
        }
        return 0; // 0 stars if not played yet
    }

    void LevelProgress::set_stars(int level_id, int stars) {
        if (stars > get_stars(level_id)) {
            m_level_stars[level_id] = stars;
            save();
        }
    }

    bool LevelProgress::is_unlocked(int level_id) const {
        if (level_id == 1) return true; // Level 1 always unlocked
        return get_stars(level_id - 1) > 0; // Unlocked if previous level has at least 1 star
    }

    int LevelProgress::calculate_stars(int coins_collected, int total_coins, int lives_remaining) const {
        int stars = 1;
        if (total_coins > 0) {
            float percentage = static_cast<float>(coins_collected) / total_coins;
            if (percentage >= 1.0f) stars = 3;
            else if (percentage >= 0.5f) stars = 2;
        } else {
            if (lives_remaining >= 3) stars = 3;
            else if (lives_remaining >= 2) stars = 2;
        }
        return stars;
    }

    void LevelProgress::add_coins(int amount) {
        m_total_coins_wallet += amount;
        save();
    }

    bool LevelProgress::spend_coins(int amount) {
        if (m_total_coins_wallet >= amount) {
            m_total_coins_wallet -= amount;
            save();
            return true;
        }
        return false;
    }

    bool LevelProgress::is_skin_unlocked(const std::string& skin_name) const {
        for (const auto& skin : m_unlocked_skins) {
            if (skin == skin_name) return true;
        }
        return false;
    }

    void LevelProgress::unlock_skin(const std::string& skin_name) {
        if (!is_skin_unlocked(skin_name)) {
            m_unlocked_skins.push_back(skin_name);
            save();
        }
    }

    void LevelProgress::select_skin(const std::string& skin_name) {
        if (is_skin_unlocked(skin_name)) {
            m_selected_skin = skin_name;
            save();
        }
    }
    
    void LevelProgress::save() {
        std::ofstream file(m_save_file, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to save progress to " << m_save_file << std::endl;
            return;
        }
        
        // Write number of entries
        int count = static_cast<int>(m_level_stars.size());
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));
        
        // Write each level's stars
        for (const auto& [level_id, stars] : m_level_stars) {
            file.write(reinterpret_cast<const char*>(&level_id), sizeof(level_id));
            file.write(reinterpret_cast<const char*>(&stars), sizeof(stars));
        }
        
        // Write wallet
        file.write(reinterpret_cast<const char*>(&m_total_coins_wallet), sizeof(m_total_coins_wallet));
        
        // Write selected skin
        int skin_len = static_cast<int>(m_selected_skin.length());
        file.write(reinterpret_cast<const char*>(&skin_len), sizeof(skin_len));
        file.write(m_selected_skin.c_str(), skin_len);
        
        // Write unlocked skins
        int unlocked_count = static_cast<int>(m_unlocked_skins.size());
        file.write(reinterpret_cast<const char*>(&unlocked_count), sizeof(unlocked_count));
        for (const auto& skin : m_unlocked_skins) {
            int len = static_cast<int>(skin.length());
            file.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file.write(skin.c_str(), len);
        }
        
        file.close();
        std::cout << "Progress saved to " << m_save_file << std::endl;
    }
    
    void LevelProgress::load() {
        std::ifstream file(m_save_file, std::ios::binary);
        if (!file) {
            std::cout << "No save file found, starting fresh" << std::endl;
            return;
        }
        
        m_level_stars.clear();
        
        // Read number of entries
        int count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));
        
        // Read each level's stars
        for (int i = 0; i < count; ++i) {
            int level_id = 0, stars = 0;
            file.read(reinterpret_cast<char*>(&level_id), sizeof(level_id));
            file.read(reinterpret_cast<char*>(&stars), sizeof(stars));
            m_level_stars[level_id] = stars;
        }
        
        // Read wallet
        if (file.peek() != EOF) {
            file.read(reinterpret_cast<char*>(&m_total_coins_wallet), sizeof(m_total_coins_wallet));
            
            // Read selected skin
            int skin_len = 0;
            file.read(reinterpret_cast<char*>(&skin_len), sizeof(skin_len));
            if (skin_len > 0) {
                char* buffer = new char[skin_len + 1];
                file.read(buffer, skin_len);
                buffer[skin_len] = '\0';
                m_selected_skin = std::string(buffer);
                delete[] buffer;
            }
            
            // Read unlocked skins
            int unlocked_count = 0;
            file.read(reinterpret_cast<char*>(&unlocked_count), sizeof(unlocked_count));
            m_unlocked_skins.clear();
            for (int i = 0; i < unlocked_count; ++i) {
                int len = 0;
                file.read(reinterpret_cast<char*>(&len), sizeof(len));
                char* buffer = new char[len + 1];
                file.read(buffer, len);
                buffer[len] = '\0';
                m_unlocked_skins.push_back(std::string(buffer));
                delete[] buffer;
            }
        }
        
        file.close();
        std::cout << "Progress loaded: " << m_level_stars.size() << " levels" << std::endl;
    }

} // namespace core
