#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../core/GameWindow.hpp"
#include <optional>

namespace entities {

    class Coin {
    public:
        Coin(sf::Vector2f position);
        
        void render(core::GameWindow& window);
        sf::FloatRect get_bounds() const;
        bool is_collected() const { return m_collected; }
        void collect() { 
            m_collected = true;
            if (m_collect_sound) {
                m_collect_sound->play();
            }
        }
        
    private:
        sf::Vector2f m_position;
        bool m_collected;
        std::optional<sf::Sprite> m_sprite;
        std::optional<sf::Sound> m_collect_sound;
    };

} // namespace entities
