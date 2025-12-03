#include "FlyingEnemy.hpp"
#include <cmath>

namespace entities {

    FlyingEnemy::FlyingEnemy(const sf::Vector2f& position)
        : Entity(position, sf::Vector2f(32.0f, 32.0f)),
          m_start_y(position.y),
          m_total_time(0.0f),
          m_frame(0),
          m_animation_timer(0.0f) {
        
        auto& rm = core::ResourceManager::instance();
        rm.load_texture("fly_a", "assets/Pack_to_pick/Game/Sprites/Enemies/Double/fly_a.png");
        rm.load_texture("fly_b", "assets/Pack_to_pick/Game/Sprites/Enemies/Double/fly_b.png");
        
        m_sprite.emplace(rm.get_texture("fly_a"));
        
        // Scale to fit
        sf::Vector2u tex_size = m_sprite->getTexture().getSize();
        m_sprite->setScale(sf::Vector2f(m_size.x / tex_size.x, m_size.y / tex_size.y));
        m_sprite->setPosition(position);
    }

    void FlyingEnemy::update(float dt) {
        m_total_time += dt;
        
        // Vertical movement
        float new_y = m_start_y + std::sin(m_total_time * SPEED) * AMPLITUDE;
        m_position.y = new_y;
        if (m_sprite) {
            m_sprite->setPosition(m_position);
        }
        
        // Animation
        m_animation_timer += dt;
        if (m_animation_timer >= ANIMATION_SPEED) {
            m_animation_timer = 0.0f;
            m_frame = 1 - m_frame;
            
            auto& rm = core::ResourceManager::instance();
            if (m_sprite) {
                if (m_frame == 0) {
                    m_sprite->setTexture(rm.get_texture("fly_a"));
                } else {
                    m_sprite->setTexture(rm.get_texture("fly_b"));
                }
            }
        }
    }

    void FlyingEnemy::render(core::GameWindow& window) {
        if (m_sprite) {
            window.draw(*m_sprite);
        }
    }

} // namespace entities
