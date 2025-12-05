#include "LevelEditorState.hpp"
#include "GameState.hpp"
#include "../core/ResourceManager.hpp"
#include "../core/GameWindow.hpp"
#include "../core/CustomLevelManager.hpp"
#include <iostream>
#include <sstream>

namespace states {

    LevelEditorState::LevelEditorState(StateManager& state_manager, int level_id)
        : m_state_manager(state_manager),
          m_level_id(level_id),
          m_background(core::ResourceManager::instance().get_texture("menu_bg")),
          m_title(core::ResourceManager::instance().get_font("cosmic_font"))
    {}

    void LevelEditorState::init() {
        const auto& bg_tex = core::ResourceManager::instance().get_texture("menu_bg");
        sf::Vector2u bg_size = bg_tex.getSize();
        m_background.setScale({1280.0f / bg_size.x, 720.0f / bg_size.y});

        // Setup Title
        std::string title_str = (m_level_id == -1) ? "Nouveau Niveau" : "Editer Niveau";
        m_title.setString(title_str);
        m_title.setCharacterSize(40);
        m_title.setFillColor(sf::Color(100, 200, 255));
        m_title.setOutlineColor(sf::Color(0, 50, 100));
        m_title.setOutlineThickness(3.0f);
        
        sf::FloatRect title_bounds = m_title.getLocalBounds();
        m_title.setOrigin({title_bounds.position.x + title_bounds.size.x / 2.0f,
                           title_bounds.position.y + title_bounds.size.y / 2.0f});
        m_title.setPosition({1280.0f / 2.0f, 50.0f});

        // Load textures
        auto& rm = core::ResourceManager::instance();
        rm.load_texture("editor_block", "assets/Pack_to_pick/Game/Sprites/Tiles/Default/terrain_grass_block.png");
        rm.load_texture("enemy_icon", "assets/Pack_to_pick/Game/Sprites/Enemies/Default/slime_normal_rest.png");
        rm.load_texture("flying_enemy_icon", "assets/Pack_to_pick/Game/Sprites/Enemies/Default/bee_rest.png");
        rm.load_texture("player_icon", "assets/gameplay/player_idle.png");
        rm.load_texture("flag_icon", "assets/gameplay/items/flag_green_a.png");
        rm.load_texture("checkpoint_icon", "assets/Pack_to_pick/Game/Sprites/Tiles/Default/switch_red.png");
        rm.load_texture("button_yellow_rect", "assets/Pack_to_pick/UI/PNG/Yellow/Default/button_rectangle_depth_flat.png");
        
        if (!rm.has_sound_buffer("tap_sound")) {
            rm.load_sound_buffer("tap_sound", "assets/Pack_to_pick/UI/Sounds/tap-a.ogg");
        }

        // Init grid
        init_grid();
        
        // Load existing level if editing
        if (m_level_id != -1) {
            load_level(m_level_id);
        }

        create_toolbar_buttons();
    }

    void LevelEditorState::create_toolbar_buttons() {
        m_toolbar_buttons.clear();
        m_action_buttons.clear();
        m_size_buttons.clear();
        
        auto& rm = core::ResourceManager::instance();
        auto& font = rm.get_font("cosmic_font");

        // Toolbar buttons - moved to the right to leave room for ANNULER
        float toolbar_y = 650.0f;
        float btn_size = 55.0f;
        float btn_wide = 75.0f;  // Wider buttons for P and F
        float btn_gap = 8.0f;
        float toolbar_start_x = 180.0f;  // Moved right to leave room for cancel

        // Block tool - with texture icon
        auto block_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{toolbar_start_x, toolbar_y}, 
            sf::Vector2f{btn_size, btn_size}, 
            "", 
            "button_blue_rect", 
            "tap_sound", 
            font
        );
        block_btn->set_icon(rm.get_texture("editor_block"));
        block_btn->set_callback([this]() { m_current_tool = EditorTool::Block; });
        m_toolbar_buttons.push_back(std::move(block_btn));

        float x_offset = btn_size + btn_gap;

        // Erase tool
        auto erase_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{toolbar_start_x + x_offset, toolbar_y}, 
            sf::Vector2f{btn_size, btn_size}, 
            "X", 
            "button_red_rect", 
            "tap_sound", 
            font
        );
        erase_btn->set_callback([this]() { m_current_tool = EditorTool::Erase; });
        m_toolbar_buttons.push_back(std::move(erase_btn));

        x_offset += btn_size + btn_gap;

        // Player tool - wider button
        auto player_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{toolbar_start_x + x_offset, toolbar_y}, 
            sf::Vector2f{btn_wide, btn_size}, 
            "", 
            "button_green_rect", 
            "tap_sound", 
            font
        );
        player_btn->set_icon(rm.get_texture("player_icon"));
        player_btn->set_callback([this]() { m_current_tool = EditorTool::Player; });
        m_toolbar_buttons.push_back(std::move(player_btn));

        x_offset += btn_wide + btn_gap;

        // Flag tool - wider button
        auto flag_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{toolbar_start_x + x_offset, toolbar_y}, 
            sf::Vector2f{btn_wide, btn_size}, 
            "", 
            "button_yellow_rect", 
            "tap_sound", 
            font
        );
        flag_btn->set_icon(rm.get_texture("flag_icon"));
        flag_btn->set_callback([this]() { m_current_tool = EditorTool::Flag; });
        m_toolbar_buttons.push_back(std::move(flag_btn));

        x_offset += btn_wide + btn_gap;

        // Enemy tool
        auto enemy_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{toolbar_start_x + x_offset, toolbar_y}, 
            sf::Vector2f{btn_size, btn_size}, 
            "", 
            "button_blue_rect", 
            "tap_sound", 
            font
        );
        enemy_btn->set_icon(rm.get_texture("enemy_icon"));
        enemy_btn->set_callback([this]() { m_current_tool = EditorTool::Enemy; });
        m_toolbar_buttons.push_back(std::move(enemy_btn));

        x_offset += btn_size + btn_gap;

        // Flying Enemy tool
        auto fly_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{toolbar_start_x + x_offset, toolbar_y}, 
            sf::Vector2f{btn_size, btn_size}, 
            "", 
            "button_blue_rect", 
            "tap_sound", 
            font
        );
        fly_btn->set_icon(rm.get_texture("flying_enemy_icon"));
        fly_btn->set_callback([this]() { m_current_tool = EditorTool::FlyingEnemy; });
        m_toolbar_buttons.push_back(std::move(fly_btn));

        x_offset += btn_size + btn_gap;

        // Checkpoint tool
        auto checkpoint_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{toolbar_start_x + x_offset, toolbar_y}, 
            sf::Vector2f{btn_size, btn_size}, 
            "", 
            "button_yellow_rect", 
            "tap_sound", 
            font
        );
        checkpoint_btn->set_icon(rm.get_texture("checkpoint_icon"));
        checkpoint_btn->set_callback([this]() { m_current_tool = EditorTool::Checkpoint; });
        m_toolbar_buttons.push_back(std::move(checkpoint_btn));

        // Cancel button - far left
        auto cancel_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{20.0f, toolbar_y}, 
            sf::Vector2f{140.f, 50.f}, 
            "ANNULER", 
            "button_cosmic", 
            "tap_sound", 
            font
        );
        cancel_btn->set_callback([this]() {
            m_state_manager.pop_state();
        });
        m_action_buttons.push_back(std::move(cancel_btn));

        // Action buttons (right side)
        float action_x = 920.0f;
        
        // Test button
        auto test_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{action_x, toolbar_y}, 
            sf::Vector2f{110.f, 50.f}, 
            "TESTER", 
            "button_green_rect", 
            "tap_sound", 
            font
        );
        test_btn->set_callback([this]() {
            if (validate_level()) {
                std::string data = generate_level_data();
                m_state_manager.push_state(std::make_unique<GameState>(m_state_manager, data, true));
            }
        });
        m_action_buttons.push_back(std::move(test_btn));

        // Save button
        auto save_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{action_x + 120.0f, toolbar_y}, 
            sf::Vector2f{110.f, 50.f}, 
            "SAUVER", 
            "button_yellow_rect", 
            "tap_sound", 
            font
        );
        save_btn->set_callback([this]() {
            if (validate_level()) {
                save_level();
                m_state_manager.pop_state();
            }
        });
        m_action_buttons.push_back(std::move(save_btn));

        // Map size buttons (top right)
        float size_y = 60.0f;
        float size_x = 1000.0f;
        
        auto small_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{size_x, size_y}, 
            sf::Vector2f{80.f, 35.f}, 
            "PETIT", 
            m_current_map_size == MapSize::Small ? "button_green_rect" : "button_blue_rect", 
            "tap_sound", 
            font
        );
        small_btn->set_callback([this]() { change_map_size(MapSize::Small); });
        m_size_buttons.push_back(std::move(small_btn));

        auto medium_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{size_x + 90.0f, size_y}, 
            sf::Vector2f{80.f, 35.f}, 
            "MOYEN", 
            m_current_map_size == MapSize::Medium ? "button_green_rect" : "button_blue_rect", 
            "tap_sound", 
            font
        );
        medium_btn->set_callback([this]() { change_map_size(MapSize::Medium); });
        m_size_buttons.push_back(std::move(medium_btn));

        auto large_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{size_x + 180.0f, size_y}, 
            sf::Vector2f{80.f, 35.f}, 
            "GRAND", 
            m_current_map_size == MapSize::Large ? "button_green_rect" : "button_blue_rect", 
            "tap_sound", 
            font
        );
        large_btn->set_callback([this]() { change_map_size(MapSize::Large); });
        m_size_buttons.push_back(std::move(large_btn));
    }

    void LevelEditorState::change_map_size(MapSize size) {
        if (m_current_map_size == size) return;
        
        m_current_map_size = size;
        
        switch (size) {
            case MapSize::Small:
                m_grid_cols = 20;
                m_grid_rows = 10;
                break;
            case MapSize::Medium:
                m_grid_cols = 40;
                m_grid_rows = 10;
                break;
            case MapSize::Large:
                m_grid_cols = 60;
                m_grid_rows = 10;
                break;
        }
        
        // Reset grid
        reset_grid();
        
        // Recreate buttons to update active state
        create_toolbar_buttons();
    }

    void LevelEditorState::init_grid() {
        reset_grid();
    }

    void LevelEditorState::reset_grid() {
        m_grid.clear();
        m_grid.resize(m_grid_rows);
        for (int row = 0; row < m_grid_rows; ++row) {
            m_grid[row].resize(m_grid_cols, ' ');
        }
        
        // Add walls on sides and bottom floor
        for (int row = 0; row < m_grid_rows; ++row) {
            m_grid[row][0] = '#';
            m_grid[row][m_grid_cols - 1] = '#';
        }
        for (int col = 0; col < m_grid_cols; ++col) {
            m_grid[m_grid_rows - 1][col] = '#';
            m_grid[m_grid_rows - 2][col] = '#';
        }
        
        // Reset entity tracking
        m_player_placed = false;
        m_flag_placed = false;
        m_player_col = m_player_row = -1;
        m_flag_col = m_flag_row = -1;
    }

    void LevelEditorState::load_level(int level_id) {
        auto level_opt = core::CustomLevelManager::instance().get_level(level_id);
        if (!level_opt) return;
        
        const std::string& data = level_opt->data;
        std::istringstream stream(data);
        std::string line;
        int row = 0;
        
        // Reset grid first
        for (auto& r : m_grid) {
            std::fill(r.begin(), r.end(), ' ');
        }
        m_player_placed = false;
        m_flag_placed = false;
        
        while (std::getline(stream, line) && row < m_grid_rows) {
            for (size_t col = 0; col < line.length() && col < static_cast<size_t>(m_grid_cols); ++col) {
                m_grid[row][col] = line[col];
                if (line[col] == 'P') {
                    m_player_placed = true;
                    m_player_col = static_cast<int>(col);
                    m_player_row = row;
                } else if (line[col] == 'F') {
                    m_flag_placed = true;
                    m_flag_col = static_cast<int>(col);
                    m_flag_row = row;
                }
            }
            row++;
        }
    }

    void LevelEditorState::place_tile(int col, int row) {
        if (col < 0 || col >= m_grid_cols || row < 0 || row >= m_grid_rows) return;
        
        char tile_char = ' ';
        
        switch (m_current_tool) {
            case EditorTool::Block:
                tile_char = '#';
                break;
            case EditorTool::Erase:
                if (m_grid[row][col] == 'P') {
                    m_player_placed = false;
                    m_player_col = m_player_row = -1;
                } else if (m_grid[row][col] == 'F') {
                    m_flag_placed = false;
                    m_flag_col = m_flag_row = -1;
                }
                tile_char = ' ';
                break;
            case EditorTool::Player:
                if (m_player_placed) {
                    m_grid[m_player_row][m_player_col] = ' ';
                }
                tile_char = 'P';
                m_player_placed = true;
                m_player_col = col;
                m_player_row = row;
                break;
            case EditorTool::Flag:
                if (m_flag_placed) {
                    m_grid[m_flag_row][m_flag_col] = ' ';
                }
                tile_char = 'F';
                m_flag_placed = true;
                m_flag_col = col;
                m_flag_row = row;
                break;
            case EditorTool::Enemy:
                tile_char = 'E';
                break;
            case EditorTool::FlyingEnemy:
                tile_char = 'V';
                break;
            case EditorTool::Checkpoint:
                tile_char = 'C';
                break;
        }
        
        m_grid[row][col] = tile_char;
    }

    void LevelEditorState::erase_tile(int col, int row) {
        if (col < 0 || col >= m_grid_cols || row < 0 || row >= m_grid_rows) return;
        
        if (m_grid[row][col] == 'P') {
            m_player_placed = false;
            m_player_col = m_player_row = -1;
        } else if (m_grid[row][col] == 'F') {
            m_flag_placed = false;
            m_flag_col = m_flag_row = -1;
        }
        
        m_grid[row][col] = ' ';
    }

    bool LevelEditorState::validate_level() {
        if (!m_player_placed) {
            std::cout << "Erreur: Placez un joueur (P)!" << std::endl;
            return false;
        }
        if (!m_flag_placed) {
            std::cout << "Erreur: Placez un drapeau (F)!" << std::endl;
            return false;
        }
        return true;
    }

    std::string LevelEditorState::generate_level_data() {
        std::ostringstream ss;
        for (int row = 0; row < m_grid_rows; ++row) {
            for (int col = 0; col < m_grid_cols; ++col) {
                ss << m_grid[row][col];
            }
            ss << "\n";
        }
        return ss.str();
    }

    void LevelEditorState::save_level() {
        core::CustomLevel level;
        
        if (m_level_id == -1) {
            level.id = core::CustomLevelManager::instance().get_next_id();
            level.name = "Niveau Custom " + std::to_string(level.id);
        } else {
            level.id = m_level_id;
            auto existing = core::CustomLevelManager::instance().get_level(m_level_id);
            level.name = existing ? existing->name : ("Niveau Custom " + std::to_string(level.id));
        }
        
        level.data = generate_level_data();
        core::CustomLevelManager::instance().save_level(level);
        
        std::cout << "Level saved: " << level.name << std::endl;
    }

    void LevelEditorState::handle_input() {
        auto mouse_pos_i = sf::Mouse::getPosition(m_state_manager.get_window().get_sf_window());
        sf::Vector2f mouse_pos(static_cast<float>(mouse_pos_i.x), static_cast<float>(mouse_pos_i.y));
        bool mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        bool right_mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

        // Update buttons
        for (auto& btn : m_toolbar_buttons) {
            btn->update(mouse_pos, mouse_pressed);
        }
        for (auto& btn : m_action_buttons) {
            btn->update(mouse_pos, mouse_pressed);
        }
        for (auto& btn : m_size_buttons) {
            btn->update(mouse_pos, mouse_pressed);
        }

        // Grid interaction - calculate dynamic tile size based on cols
        float tile_size = std::min(TILE_SIZE, (1200.0f - GRID_START_X) / m_grid_cols);
        float grid_end_x = GRID_START_X + m_grid_cols * tile_size;
        float grid_end_y = GRID_START_Y + m_grid_rows * tile_size;
        
        if (mouse_pos.x >= GRID_START_X && mouse_pos.x < grid_end_x &&
            mouse_pos.y >= GRID_START_Y && mouse_pos.y < grid_end_y) {
            
            int col = static_cast<int>((mouse_pos.x - GRID_START_X) / tile_size);
            int row = static_cast<int>((mouse_pos.y - GRID_START_Y) / tile_size);
            
            if (mouse_pressed) {
                place_tile(col, row);
            } else if (right_mouse_pressed) {
                erase_tile(col, row);
            }
        }
        
        m_was_mouse_pressed = mouse_pressed;
    }

    void LevelEditorState::update(float dt) {
        (void)dt;
    }

    void LevelEditorState::draw(core::GameWindow& window) {
        // Draw solid blue background
        window.clear(sf::Color(50, 120, 180));
        window.draw(m_title);
        
        auto& rm = core::ResourceManager::instance();
        const auto& tile_tex = rm.get_texture("editor_block");
        const auto& player_tex = rm.get_texture("player_icon");
        const auto& flag_tex = rm.get_texture("flag_icon");
        const auto& enemy_tex = rm.get_texture("enemy_icon");
        const auto& fly_tex = rm.get_texture("flying_enemy_icon");
        const auto& checkpoint_tex = rm.get_texture("checkpoint_icon");

        // Calculate dynamic tile size
        float tile_size = std::min(TILE_SIZE, (1200.0f - GRID_START_X) / m_grid_cols);
        
        // Draw grid background
        sf::RectangleShape grid_bg;
        grid_bg.setPosition({GRID_START_X, GRID_START_Y});
        grid_bg.setSize({m_grid_cols * tile_size, m_grid_rows * tile_size});
        grid_bg.setFillColor(sf::Color(30, 30, 50, 200));
        grid_bg.setOutlineColor(sf::Color(100, 100, 150));
        grid_bg.setOutlineThickness(2.0f);
        window.draw(grid_bg);
        
        // Draw grid lines
        for (int col = 0; col <= m_grid_cols; ++col) {
            sf::RectangleShape line;
            line.setPosition({GRID_START_X + col * tile_size, GRID_START_Y});
            line.setSize({1.0f, m_grid_rows * tile_size});
            line.setFillColor(sf::Color(60, 60, 80));
            window.draw(line);
        }
        for (int row = 0; row <= m_grid_rows; ++row) {
            sf::RectangleShape line;
            line.setPosition({GRID_START_X, GRID_START_Y + row * tile_size});
            line.setSize({m_grid_cols * tile_size, 1.0f});
            line.setFillColor(sf::Color(60, 60, 80));
            window.draw(line);
        }
        
        // Draw tiles
        for (int row = 0; row < m_grid_rows; ++row) {
            for (int col = 0; col < m_grid_cols; ++col) {
                char c = m_grid[row][col];
                if (c == ' ') continue;
                
                float x = GRID_START_X + col * tile_size;
                float y = GRID_START_Y + row * tile_size;
                
                const sf::Texture* texture = nullptr;
                switch (c) {
                    case '#': texture = &tile_tex; break;
                    case 'P': texture = &player_tex; break;
                    case 'F': texture = &flag_tex; break;
                    case 'E': texture = &enemy_tex; break;
                    case 'V': texture = &fly_tex; break;
                    case 'C': texture = &checkpoint_tex; break;
                    default: continue;
                }
                
                if (!texture) continue;
                
                sf::Sprite sprite(*texture);
                sf::Vector2u tex_size = texture->getSize();
                sprite.setScale({tile_size / tex_size.x, tile_size / tex_size.y});
                sprite.setPosition({x, y});
                window.draw(sprite);
            }
        }
        
        // Draw current tool indicator
        auto& font = rm.get_font("cosmic_font");
        std::string tool_name;
        switch (m_current_tool) {
            case EditorTool::Block: tool_name = "Bloc"; break;
            case EditorTool::Erase: tool_name = "Effacer"; break;
            case EditorTool::Player: tool_name = "Joueur"; break;
            case EditorTool::Flag: tool_name = "Drapeau"; break;
            case EditorTool::Enemy: tool_name = "Ennemi Sol"; break;
            case EditorTool::FlyingEnemy: tool_name = "Ennemi Volant"; break;
            case EditorTool::Checkpoint: tool_name = "Checkpoint"; break;
        }
        
        sf::Text tool_text(font, "Outil: " + tool_name, 20);
        tool_text.setFillColor(sf::Color::White);
        tool_text.setPosition({GRID_START_X, GRID_START_Y + m_grid_rows * tile_size + 10.0f});
        window.draw(tool_text);
        
        // Draw map size indicator
        std::string size_name;
        switch (m_current_map_size) {
            case MapSize::Small: size_name = "Petit (20x10)"; break;
            case MapSize::Medium: size_name = "Moyen (40x10)"; break;
            case MapSize::Large: size_name = "Grand (60x10)"; break;
        }
        sf::Text size_text(font, "Taille: " + size_name, 18);
        size_text.setFillColor(sf::Color(200, 255, 200));
        size_text.setPosition({GRID_START_X + 200.0f, GRID_START_Y + m_grid_rows * tile_size + 10.0f});
        window.draw(size_text);
        
        // Draw validation hints
        sf::Text hint_text(font, "", 18);
        hint_text.setFillColor(sf::Color(255, 200, 100));
        std::string hints;
        if (!m_player_placed) hints += "Placez un joueur (P)  ";
        if (!m_flag_placed) hints += "Placez un drapeau (F)";
        hint_text.setString(hints);
        hint_text.setPosition({GRID_START_X, GRID_START_Y + m_grid_rows * tile_size + 35.0f});
        window.draw(hint_text);
        
        // Draw buttons
        for (auto& btn : m_toolbar_buttons) {
            btn->render(window);
        }
        for (auto& btn : m_action_buttons) {
            btn->render(window);
        }
        for (auto& btn : m_size_buttons) {
            btn->render(window);
        }
    }

} // namespace states
