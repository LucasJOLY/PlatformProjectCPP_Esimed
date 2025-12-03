#include "SkinManager.hpp"

namespace core {

    SkinManager& SkinManager::instance() {
        static SkinManager instance;
        return instance;
    }

    SkinManager::SkinManager() {
        // Define available skins
        m_skins = {
            {"beige", "Beige", 0, "character_beige", sf::Color(245, 245, 220)},
            {"green", "Green", 5, "character_green", sf::Color(144, 238, 144)},
            {"pink", "Pink", 5, "character_pink", sf::Color(255, 182, 193)},
            {"purple", "Purple", 5, "character_purple", sf::Color(221, 160, 221)},
            {"yellow", "Yellow", 5, "character_yellow", sf::Color(255, 255, 224)}
        };
    }

    const std::vector<SkinInfo>& SkinManager::get_all_skins() const {
        return m_skins;
    }

    const SkinInfo& SkinManager::get_skin(const std::string& id) const {
        for (const auto& skin : m_skins) {
            if (skin.id == id) return skin;
        }
        return m_skins[0]; // Return default if not found
    }

} // namespace core
