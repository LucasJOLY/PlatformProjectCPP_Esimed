#include "Enemy.hpp"

namespace entities {

    Enemy::Enemy(const sf::Vector2f& position)
        : Entity(position, sf::Vector2f(32.0f, 32.0f)),
          m_direction(1.0f),
          m_animation_timer(0.0f),
          m_walk_frame(0) {
        
        // Load slime textures from the pack
        auto& rm = core::ResourceManager::instance();
        rm.load_texture("slime_walk_a", "assets/Pack_to_pick/Game/Sprites/Enemies/Default/slime_normal_walk_a.png");
        rm.load_texture("slime_walk_b", "assets/Pack_to_pick/Game/Sprites/Enemies/Default/slime_normal_walk_b.png");
        
        m_sprite.emplace(rm.get_texture("slime_walk_a"));
        
        // Scale sprite to match hitbox
        sf::Vector2u tex_size = m_sprite->getTexture().getSize();
        m_sprite->setScale(sf::Vector2f(m_size.x / tex_size.x, m_size.y / tex_size.y));
        m_sprite->setPosition(position);
    }

    void Enemy::update(float dt) {
        // Simple patrol: move left/right
        m_velocity.x = m_direction * MOVE_SPEED;
        
        // Update position
        m_position += m_velocity * dt;
        
        // Animation
        m_animation_timer += dt;
        if (m_animation_timer >= ANIMATION_SPEED) {
            m_animation_timer = 0.0f;
            m_walk_frame = 1 - m_walk_frame;
            
            auto& rm = core::ResourceManager::instance();
            if (m_sprite) {
                if (m_walk_frame == 0) {
                    m_sprite->setTexture(rm.get_texture("slime_walk_a"));
                } else {
                    m_sprite->setTexture(rm.get_texture("slime_walk_b"));
                }
            }
        }
        
        // Update sprite position and flip based on direction
        if (m_sprite) {
            m_sprite->setPosition(m_position);
            
            // Flip sprite based on direction
            sf::Vector2f scale = m_sprite->getScale();
            scale.x = std::abs(scale.x) * (m_direction > 0 ? 1.0f : -1.0f);
            m_sprite->setScale(scale);
            
            // Adjust origin for flipping
            if (m_direction < 0) {
                m_sprite->setOrigin(sf::Vector2f(m_sprite->getTexture().getSize().x, 0.0f));
            } else {
                m_sprite->setOrigin(sf::Vector2f(0.0f, 0.0f));
            }
        }
    }

    void Enemy::render(core::GameWindow& window) {
        if (m_sprite) {
            window.draw(*m_sprite);
        }
    }

    void Enemy::check_wall_collision(const std::vector<sf::FloatRect>& solid_tiles) {
        sf::FloatRect enemy_bounds = get_bounds();
        
        for (const auto& tile_bounds : solid_tiles) {
            if (enemy_bounds.findIntersection(tile_bounds)) {
                // Hit a wall, turn around
                m_direction *= -1.0f;
                
                // Move away from wall
                if (m_direction > 0) {
                    m_position.x = tile_bounds.position.x + tile_bounds.size.x + 1.0f;
                } else {
                    m_position.x = tile_bounds.position.x - m_size.x - 1.0f;
                }
                break;
            }
        }
    }

} // namespace entities
