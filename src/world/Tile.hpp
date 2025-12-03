#pragma once

#include <SFML/Graphics.hpp>
#include <optional>

namespace world {

    enum class TileType {
        EMPTY,
        SOLID,
        CHECKPOINT,
        FLAG
    };

    struct Tile {
        TileType type;
        sf::Vector2f position;
        std::optional<sf::Sprite> sprite;
        
        Tile() : type(TileType::EMPTY), position(0.0f, 0.0f), sprite(std::nullopt) {}
        Tile(TileType t, const sf::Vector2f& pos) : type(t), position(pos), sprite(std::nullopt) {}
        
        sf::FloatRect get_bounds() const {
            return sf::FloatRect(position, sf::Vector2f(32.0f, 32.0f));
        }
    };

} // namespace world
