#pragma once

#include "Entity.hpp"
#include "../core/ResourceManager.hpp"

namespace entities {

    class Enemy : public Entity {
    public:
        Enemy(const sf::Vector2f& position);
        ~Enemy() override = default;

        void update(float dt) override;
        void render(core::GameWindow& window) override;

        void check_wall_collision(const std::vector<sf::FloatRect>& solid_tiles);

    private:
        sf::Sprite m_sprite;
        float m_direction; // 1.0 = right, -1.0 = left
        
        static constexpr float MOVE_SPEED = 80.0f;
    };

} // namespace entities
