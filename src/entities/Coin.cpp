#include "Coin.hpp"
#include "../core/ResourceManager.hpp"
#include <iostream>

namespace entities {

    Coin::Coin(sf::Vector2f position) : m_position(position), m_collected(false) {
        // Load coin texture
        auto& texture = core::ResourceManager::instance().load_texture("coin_gold", "assets/gameplay/items/coin_gold.png");
        m_sprite = sf::Sprite(texture);
        
        // Scale coin to fit tile size (32x32)
        sf::Vector2u tex_size = texture.getSize();
        float scale_x = 24.0f / tex_size.x;  // Slightly smaller than tile
        float scale_y = 24.0f / tex_size.y;
        m_sprite->setScale(sf::Vector2f(scale_x, scale_y));
        
        // Center coin in tile
        m_sprite->setPosition(sf::Vector2f(m_position.x + 4.0f, m_position.y + 4.0f));
        
        // Load collection sound
        auto& rm = core::ResourceManager::instance();
        rm.load_sound_buffer("coin_collect", "assets/Pack_to_pick/Game/Sounds/sfx_coin.ogg");
        m_collect_sound.emplace(rm.get_sound_buffer("coin_collect"));
    }
    
    void Coin::render(core::GameWindow& window) {
        if (!m_collected && m_sprite) {
            window.draw(*m_sprite);
        }
    }
    
    sf::FloatRect Coin::get_bounds() const {
        return sf::FloatRect(sf::Vector2f(m_position.x + 4.0f, m_position.y + 4.0f), sf::Vector2f(24.0f, 24.0f));
    }

} // namespace entities
