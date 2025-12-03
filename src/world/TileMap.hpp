#pragma once

#include "Tile.hpp"
#include "../core/ResourceManager.hpp"
#include "../core/GameWindow.hpp"
#include <vector>
#include <string>

namespace world {

    class TileMap {
    public:
        TileMap();
        ~TileMap() = default;

        void load_from_string(const std::string& level_data, int level_id);
        void render(core::GameWindow& window);
        sf::Vector2f get_spawn_position() const { return m_spawn_position; }
        sf::Vector2f get_flag_position() const { return m_flag_position; }
        std::vector<Tile> get_solid_tiles() const;
        int get_width() const { return m_tiles.empty() ? 0 : static_cast<int>(m_tiles[0].size()); }
        int get_height() const { return static_cast<int>(m_tiles.size()); }
        std::vector<sf::Vector2f> get_checkpoint_positions() const { return m_checkpoint_positions; }

    private:
        std::vector<std::vector<Tile>> m_tiles;
        sf::Vector2f m_spawn_position;
        sf::Vector2f m_flag_position;
        std::vector<sf::Vector2f> m_checkpoint_positions;
        std::optional<sf::Sprite> m_flag_sprite;
        
        static constexpr float TILE_SIZE = 32.0f;
    };

} // namespace world
