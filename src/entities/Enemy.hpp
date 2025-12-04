#pragma once

#include "Entity.hpp"
#include "../core/ResourceManager.hpp"
#include <optional>

namespace entities {

    class Enemy : public Entity {
    public:
        Enemy(const sf::Vector2f& position);
        ~Enemy() override = default;

        void update(float dt) override;
        void render(core::GameWindow& window) override;

        void check_wall_collision(const std::vector<sf::FloatRect>& solid_tiles);

    private:
        std::optional<sf::Sprite> m_sprite;
        float m_direction; // 1.0 = right, -1.0 = left
        float m_animation_timer;
        int m_walk_frame; // 0 or 1 for walk animation
        
        static constexpr float MOVE_SPEED = 80.0f;
        static constexpr float ANIMATION_SPEED = 0.15f;
    };

} // namespace entities
