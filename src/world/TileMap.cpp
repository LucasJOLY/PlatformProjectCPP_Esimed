#include "TileMap.hpp"
#include <sstream>
#include <iostream>

namespace world {

    TileMap::TileMap() : m_spawn_position(100.0f, 500.0f), m_flag_position(0.0f, 0.0f) {}

    void TileMap::load_from_string(const std::string& level_data, int level_id) {
        (void)level_id;
        m_tiles.clear();
        m_checkpoint_positions.clear();
        
        std::istringstream stream(level_data);
        std::string line;
        int row = 0;
        
        // Load grass tile texture
        auto& tile_tex = core::ResourceManager::instance().load_texture("grass_tile", 
            "assets/gameplay/tiles/terrain_grass_block.png");
        
        while (std::getline(stream, line)) {
            std::vector<Tile> tile_row;
            
            for (size_t col = 0; col < line.length(); ++col) {
                char c = line[col];
                sf::Vector2f pos(col * TILE_SIZE, row * TILE_SIZE);
                Tile tile;
                
                switch (c) {
                    case '#': // Solid block
                        tile = Tile(TileType::SOLID, pos);
                        tile.sprite = sf::Sprite(tile_tex);
                        tile.sprite->setPosition(pos);
                        break;
                    case 'P': // Player spawn
                        m_spawn_position = pos;
                        tile = Tile(TileType::EMPTY, pos);
                        break;
                    case 'C': // Checkpoint
                        m_checkpoint_positions.push_back(pos);
                        tile = Tile(TileType::CHECKPOINT, pos);
                        break;
                    case 'F': // Flag (end)
                        m_flag_position = pos;
                        tile = Tile(TileType::FLAG, pos);
                        break;
                    default: // Empty
                        tile = Tile(TileType::EMPTY, pos);
                        break;
                }
                
                tile_row.push_back(tile);
            }
            
            m_tiles.push_back(tile_row);
            row++;
        }
        
        // Load flag sprite
        auto& flag_tex = core::ResourceManager::instance().load_texture("flag_green", "assets/gameplay/items/flag_green_a.png");
        m_flag_sprite = sf::Sprite(flag_tex);
        m_flag_sprite->setPosition(m_flag_position);
        
        std::cout << "Loaded level with " << m_tiles.size() << " rows" << std::endl;
    }

    void TileMap::render(core::GameWindow& window) {
        for (const auto& row : m_tiles) {
            for (const auto& tile : row) {
                if (tile.type == TileType::SOLID && tile.sprite.has_value()) {
                    window.draw(tile.sprite.value());
                }
            }
        }
        
        // Render flag
        if (m_flag_sprite) {
            window.draw(*m_flag_sprite);
        }
    }

    std::vector<Tile> TileMap::get_solid_tiles() const {
        std::vector<Tile> solid_tiles;
        for (const auto& row : m_tiles) {
            for (const auto& tile : row) {
                if (tile.type == TileType::SOLID) {
                    solid_tiles.push_back(tile);
                }
            }
        }
        return solid_tiles;
    }

} // namespace world
