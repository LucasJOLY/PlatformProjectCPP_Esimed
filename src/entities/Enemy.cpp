#include "Enemy.hpp"

namespace entities {

    Enemy::Enemy(const sf::Vector2f& position)
        : Entity(position, sf::Vector2f(32.0f, 32.0f)),
          m_sprite(core::ResourceManager::instance().get_texture("enemy_slime")),
          m_direction(1.0f) {
        
        // Scale sprite to match hitbox
        const auto& tex = core::ResourceManager::instance().get_texture("enemy_slime");
        sf::Vector2u tex_size = tex.getSize();
        m_sprite.setScale(sf::Vector2f(m_size.x / tex_size.x, m_size.y / tex_size.y));
    }

    void Enemy::update(float dt) {
        // Simple patrol: move left/right
        m_velocity.x = m_direction * MOVE_SPEED;
        
        // Update position
        m_position += m_velocity * dt;
        
        // Update sprite
        m_sprite.setPosition(m_position);
    }

    void Enemy::render(core::GameWindow& window) {
        window.draw(m_sprite);
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
