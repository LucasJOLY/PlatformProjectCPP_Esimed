#pragma once

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

namespace core {

    struct SkinInfo {
        std::string id;
        std::string name;
        int cost;
        std::string texture_prefix; // e.g., "character_beige"
        sf::Color color_preview;
    };

    class SkinManager {
    public:
        static SkinManager& instance();
        
        const std::vector<SkinInfo>& get_all_skins() const;
        const SkinInfo& get_skin(const std::string& id) const;
        
    private:
        SkinManager();
        ~SkinManager() = default;
        
        std::vector<SkinInfo> m_skins;
    };

} // namespace core
