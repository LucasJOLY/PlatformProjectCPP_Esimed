#include "Entity.hpp"

namespace entities {

    Entity::Entity(const sf::Vector2f& position, const sf::Vector2f& size)
        : m_position(position), m_velocity(0.0f, 0.0f), m_size(size) {}

    sf::FloatRect Entity::get_bounds() const {
        return sf::FloatRect(m_position, m_size);
    }

} // namespace entities
