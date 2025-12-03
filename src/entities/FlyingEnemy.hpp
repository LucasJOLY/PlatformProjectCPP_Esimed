#pragma once

#include "Entity.hpp"
#include "../core/ResourceManager.hpp"

namespace entities {

    class FlyingEnemy : public Entity {
    public:
        FlyingEnemy(const sf::Vector2f& position);
        ~FlyingEnemy() override = default;

        void update(float dt) override;
        void render(core::GameWindow& window) override;

#include <optional>

    private:
        std::optional<sf::Sprite> m_sprite;
        float m_start_y;
        float m_total_time;
        int m_frame;
        float m_animation_timer;

        static constexpr float AMPLITUDE = 50.0f; // Vertical range
        static constexpr float SPEED = 2.0f;      // Oscillation speed
        static constexpr float ANIMATION_SPEED = 0.1f;
    };

} // namespace entities
