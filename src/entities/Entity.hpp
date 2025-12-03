#pragma once

#include <SFML/Graphics.hpp>
#include "../core/GameWindow.hpp"

namespace entities {

    class Entity {
    public:
        Entity(const sf::Vector2f& position, const sf::Vector2f& size);
        virtual ~Entity() = default;

        virtual void update(float dt) = 0;
        virtual void render(core::GameWindow& window) = 0;

        sf::FloatRect get_bounds() const;
        sf::Vector2f get_position() const { return m_position; }
        sf::Vector2f get_velocity() const { return m_velocity; }
        
        void set_position(const sf::Vector2f& pos) { m_position = pos; }
        void set_velocity(const sf::Vector2f& vel) { m_velocity = vel; }

    protected:
        sf::Vector2f m_position;
        sf::Vector2f m_velocity;
        sf::Vector2f m_size;
    };

} // namespace entities
