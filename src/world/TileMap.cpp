#include "TileMap.hpp"
#include <sstream>
#include <iostream>

namespace world {

    TileMap::TileMap() : m_spawn_position(100.0f, 500.0f), m_flag_position(0.0f, 0.0f) {}

    void TileMap::load_from_string(const std::string& level_data, int level_id) {
        m_tiles.clear();
        m_checkpoint_positions.clear();
        m_underground_sprites.clear();
        m_checkpoint_sprites.clear();
        m_activated_checkpoints.clear();
        
        std::istringstream stream(level_data);
        std::string line;
        int row = 0;
        
        // Load grass tile texture
        auto& tile_tex = core::ResourceManager::instance().load_texture("grass_tile", 
            "assets/gameplay/tiles/terrain_grass_block.png");
        
        // Load underground texture (same for all layers)
        auto& grass_bottom_tex = core::ResourceManager::instance().load_texture("grass_bottom",
            "assets/Pack_to_pick/Game/Sprites/Tiles/Default/terrain_grass_block_bottom.png");
        
        // Load checkpoint textures
        auto& checkpoint_tex = core::ResourceManager::instance().load_texture("checkpoint",
            "assets/Pack_to_pick/Game/Sprites/Tiles/Default/switch_red.png");
        auto& checkpoint_active_tex = core::ResourceManager::instance().load_texture("checkpoint_active",
            "assets/Pack_to_pick/Game/Sprites/Tiles/Default/switch_red_pressed.png");
        
        // Load background based on level
        std::string bg_path;
        switch (level_id) {
            case 1: bg_path = "assets/Pack_to_pick/Game/Sprites/Backgrounds/Default/background_color_hills.png"; break;
            case 2: bg_path = "assets/Pack_to_pick/Game/Sprites/Backgrounds/Default/background_color_trees.png"; break;
            case 3: bg_path = "assets/Pack_to_pick/Game/Sprites/Backgrounds/Default/background_color_mushrooms.png"; break;
            case 4: bg_path = "assets/Pack_to_pick/Game/Sprites/Backgrounds/Default/background_color_desert.png"; break;
            case 5: bg_path = "assets/Pack_to_pick/Game/Sprites/Backgrounds/Default/background_fade_trees.png"; break;
            default: bg_path = "assets/Pack_to_pick/Game/Sprites/Backgrounds/Default/background_color_hills.png"; break;
        }
        auto& bg_tex = core::ResourceManager::instance().load_texture("background_" + std::to_string(level_id), bg_path);
        m_background_sprite = sf::Sprite(bg_tex);
        // Scale background to cover full screen (800x600)
        auto bg_size = bg_tex.getSize();
        float bg_scale_x = 800.0f / static_cast<float>(bg_size.x);
        float bg_scale_y = 600.0f / static_cast<float>(bg_size.y);
        m_background_sprite->setScale({bg_scale_x, bg_scale_y});
        
        // Count total columns for level width
        int max_cols = 0;
        std::vector<std::string> lines;
        while (std::getline(stream, line)) {
            lines.push_back(line);
            if (static_cast<int>(line.length()) > max_cols) {
                max_cols = static_cast<int>(line.length());
            }
        }
        
        // Process tiles
        for (const auto& l : lines) {
            std::vector<Tile> tile_row;
            
            for (size_t col = 0; col < l.length(); ++col) {
                char c = l[col];
                sf::Vector2f pos(col * TILE_SIZE, row * TILE_SIZE);
                Tile tile;
                
                switch (c) {
                    case '#': // Solid block
                        tile = Tile(TileType::SOLID, pos);
                        tile.sprite = sf::Sprite(tile_tex);
                        tile.sprite->setPosition(pos);
                        {
                            // Scale sprite to fit TILE_SIZE (32x32)
                            auto texSize = tile_tex.getSize();
                            float scaleX = TILE_SIZE / static_cast<float>(texSize.x);
                            float scaleY = TILE_SIZE / static_cast<float>(texSize.y);
                            tile.sprite->setScale({scaleX, scaleY});
                        }
                        break;
                    case 'P': // Player spawn
                        m_spawn_position = pos;
                        tile = Tile(TileType::EMPTY, pos);
                        break;
                    case 'C': // Checkpoint
                        m_checkpoint_positions.push_back(pos);
                        tile = Tile(TileType::CHECKPOINT, pos);
                        {
                            // Create checkpoint sprite (initially not activated)
                            sf::Sprite cp_sprite(checkpoint_tex);
                            cp_sprite.setPosition(pos);
                            auto cp_size = checkpoint_tex.getSize();
                            float cp_scaleX = TILE_SIZE / static_cast<float>(cp_size.x);
                            float cp_scaleY = TILE_SIZE / static_cast<float>(cp_size.y);
                            cp_sprite.setScale({cp_scaleX, cp_scaleY});
                            m_checkpoint_sprites.push_back(cp_sprite);
                        }
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
        
        // Create underground sprites below the level to fill until bottom of screen (600px)
        int level_height = static_cast<int>(m_tiles.size());
        m_level_width = max_cols * TILE_SIZE;
        
        // Calculate how many underground rows needed to reach 600px (screen height)
        float level_pixel_height = level_height * TILE_SIZE;
        int underground_rows = static_cast<int>((600.0f - level_pixel_height) / TILE_SIZE) + 2; // +2 for safety margin
        if (underground_rows < 1) underground_rows = 1;
        
        for (int depth = 0; depth < underground_rows; ++depth) {
            for (int col = 0; col < max_cols; ++col) {
                sf::Vector2f pos(col * TILE_SIZE, (level_height + depth) * TILE_SIZE);
                
                // Use same texture for all underground layers
                sf::Sprite underground_sprite(grass_bottom_tex);
                underground_sprite.setPosition(pos);
                // Scale to fit TILE_SIZE
                auto texSize = grass_bottom_tex.getSize();
                float scaleX = TILE_SIZE / static_cast<float>(texSize.x);
                float scaleY = TILE_SIZE / static_cast<float>(texSize.y);
                underground_sprite.setScale({scaleX, scaleY});
                
                m_underground_sprites.push_back(underground_sprite);
            }
        }
        
        // Load flag sprite with correct texture
        auto& flag_tex = core::ResourceManager::instance().load_texture("flag_yellow", 
            "assets/Pack_to_pick/Game/Sprites/Tiles/Default/flag_yellow_a.png");
        m_flag_sprite = sf::Sprite(flag_tex);
        m_flag_sprite->setPosition(m_flag_position);
        // Scale flag to fit tile
        auto flag_size = flag_tex.getSize();
        float flag_scaleX = TILE_SIZE / static_cast<float>(flag_size.x);
        float flag_scaleY = TILE_SIZE / static_cast<float>(flag_size.y);
        m_flag_sprite->setScale({flag_scaleX, flag_scaleY});
        
        std::cout << "Loaded level " << level_id << " with " << m_tiles.size() << " rows and background" << std::endl;
    }

    void TileMap::render(core::GameWindow& window, const sf::View& camera) {
        // Render background first, positioned based on camera
        if (m_background_sprite) {
            // Get camera center and calculate background position
            sf::Vector2f camera_center = camera.getCenter();
            sf::Vector2f camera_size = camera.getSize();
            
            // Position background to follow camera (left edge of view)
            float bg_x = camera_center.x - camera_size.x / 2.0f;
            m_background_sprite->setPosition({bg_x, 0.0f});
            
            window.draw(*m_background_sprite);
        }
        
        // Render underground sprites
        for (const auto& sprite : m_underground_sprites) {
            window.draw(sprite);
        }
        
        // Render tiles
        for (const auto& row : m_tiles) {
            for (const auto& tile : row) {
                if (tile.type == TileType::SOLID && tile.sprite.has_value()) {
                    window.draw(tile.sprite.value());
                }
            }
        }
        
        // Render checkpoints
        for (const auto& cp_sprite : m_checkpoint_sprites) {
            window.draw(cp_sprite);
        }
        
        // Render flag
        if (m_flag_sprite) {
            window.draw(*m_flag_sprite);
        }
    }
    
    void TileMap::activate_checkpoint(const sf::Vector2f& position) {
        // Find the checkpoint index at this position
        for (size_t i = 0; i < m_checkpoint_positions.size(); ++i) {
            if (m_checkpoint_positions[i] == position && m_activated_checkpoints.find(i) == m_activated_checkpoints.end()) {
                // Mark as activated
                m_activated_checkpoints.insert(i);
                
                // Change texture to pressed
                auto& checkpoint_active_tex = core::ResourceManager::instance().get_texture("checkpoint_active");
                if (i < m_checkpoint_sprites.size()) {
                    m_checkpoint_sprites[i].setTexture(checkpoint_active_tex);
                }
                break;
            }
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
