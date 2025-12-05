#include "GameState.hpp"
#include "../world/World.hpp"
#include "../core/LevelProgress.hpp"
#include <iostream>
#include <memory>

namespace states {

    GameState::GameState(StateManager& state_manager, int level_id)
        : m_state_manager(state_manager), m_level_id(level_id), m_is_test_mode(false) {}

    GameState::GameState(StateManager& state_manager, const std::string& custom_data, bool is_test_mode)
        : m_state_manager(state_manager), m_level_id(-1), m_custom_data(custom_data), m_is_test_mode(is_test_mode) {}

    void GameState::init() {
        std::cout << "Initializing GameState for Level " << m_level_id << std::endl;
        
        // Preload textures
        core::ResourceManager::instance().load_texture("player_idle", "assets/gameplay/player_idle.png");
        core::ResourceManager::instance().load_texture("enemy_slime", "assets/gameplay/enemy_slime.png");
        
        // Load UI textures - using heart sprites
        auto& life_full_tex = core::ResourceManager::instance().load_texture("life_full", "assets/Pack_to_pick/Game/Sprites/Tiles/Default/hud_heart.png");
        auto& life_empty_tex = core::ResourceManager::instance().load_texture("life_empty", "assets/Pack_to_pick/Game/Sprites/Tiles/Default/hud_heart_empty.png");
        auto& panel_tex = core::ResourceManager::instance().load_texture("panel_blue", "assets/ui/panel_blue.png");
        
        // Load NEW UI textures for improved menus
        core::ResourceManager::instance().load_texture("btn_blue", "assets/Pack_to_pick/UI/PNG/Blue/Default/button_rectangle_depth_gloss.png");
        core::ResourceManager::instance().load_texture("icon_repeat", "assets/Pack_to_pick/UI/PNG/Extra/Default/icon_repeat_light.png");
        core::ResourceManager::instance().load_texture("icon_play", "assets/Pack_to_pick/UI/PNG/Extra/Default/icon_play_light.png");
        core::ResourceManager::instance().load_texture("divider", "assets/Pack_to_pick/UI/PNG/Extra/Default/divider.png");
        core::ResourceManager::instance().load_texture("star_filled", "assets/Pack_to_pick/UI/PNG/Blue/Default/star.png");
        core::ResourceManager::instance().load_texture("star_empty", "assets/Pack_to_pick/UI/PNG/Blue/Default/star_outline.png");
        
        // Setup life sprites (3 lives max)
        for (int i = 0; i < 3; ++i) {
            sf::Sprite life_sprite(life_full_tex);
            life_sprite.setScale(sf::Vector2f(0.5f, 0.5f)); // Scale down stars
            life_sprite.setPosition(sf::Vector2f(10.0f + i * 40.0f, 10.0f));
            m_life_sprites.push_back(life_sprite);
        }
        
        // Setup panel for game over/victory (centered) - make it bigger
        m_panel_sprite = sf::Sprite(panel_tex);
        m_panel_sprite->setScale({5.0f, 4.5f}); 
        // Center panel dynamically
        sf::FloatRect bounds = m_panel_sprite->getLocalBounds();
        m_panel_sprite->setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        m_panel_sprite->setPosition({1280.0f / 2.0f, 720.0f / 2.0f});
        
        // Setup divider sprite
        auto& divider_tex = core::ResourceManager::instance().get_texture("divider");
        m_divider_sprite = sf::Sprite(divider_tex);
        m_divider_sprite->setScale({5.0f, 2.0f});
        sf::FloatRect div_bounds = m_divider_sprite->getLocalBounds();
        m_divider_sprite->setOrigin({div_bounds.size.x / 2.0f, div_bounds.size.y / 2.0f});
        m_divider_sprite->setPosition({1280.0f / 2.0f, 330.0f});
        
        // Load font for HUD
        auto& font = core::ResourceManager::instance().load_font("cosmic_font", "assets/menu/font_cosmic.ttf");
        
        // Setup HUD text
        m_status_text = sf::Text(font);
        m_status_text->setCharacterSize(36);
        m_status_text->setFillColor(sf::Color::White);
        // Position will be set in draw
        
        // Load sounds - NEW SOUND ASSETS
        if (!m_jump_buffer.loadFromFile("assets/Pack_to_pick/Game/Sounds/sfx_jump.ogg")) {
            std::cerr << "Failed to load jump sound" << std::endl;
        }
        if (!m_damage_buffer.loadFromFile("assets/Pack_to_pick/Game/Sounds/sfx_bump.ogg")) {
            std::cerr << "Failed to load damage sound" << std::endl;
        }
        if (!m_victory_buffer.loadFromFile("assets/sounds/victory.ogg")) {
            std::cerr << "Failed to load victory sound" << std::endl;
        }
        
        m_jump_sound = sf::Sound(m_jump_buffer);
        m_damage_sound = sf::Sound(m_damage_buffer);
        m_victory_sound = sf::Sound(m_victory_buffer);
        
        // Create world - use custom data if available, otherwise use level_id
        if (!m_custom_data.empty()) {
            m_world = std::make_unique<world::World>(m_custom_data);
        } else {
            m_world = std::make_unique<world::World>(m_level_id);
        }
    }

    void GameState::create_menu_button(bool is_victory) {
        auto& font = core::ResourceManager::instance().get_font("cosmic_font");
        
        std::string label;
        std::string icon_name;
        
        if (is_victory) {
            if (m_level_id < 5) {
                label = "Next Level";
                icon_name = "icon_play";
            } else {
                label = "Back to Menu";
                icon_name = "icon_repeat";
            }
        } else {
            label = "Retry";
            icon_name = "icon_repeat";
        }
        
        // Create centered button
        sf::Vector2f btn_size(220.0f, 50.0f);
        sf::Vector2f btn_pos(1280.0f / 2.0f - btn_size.x / 2.0f, 400.0f);
        
        m_action_button = std::make_unique<ui::UIButton>(
            btn_pos, btn_size, label, "btn_blue", "click", font
        );
        
        // Set icon
        auto& icon_tex = core::ResourceManager::instance().get_texture(icon_name);
        m_action_button->set_icon(icon_tex);
        
        // Set callback
        m_action_button->set_callback([this, is_victory]() {
            if (is_victory && m_level_id > 0 && m_level_id < 5) {
                // Advance to next level
                m_level_id++;
                m_world = std::make_unique<world::World>(m_level_id);
                m_action_button.reset();
                m_menu_shown = false;
            } else if (is_victory && (m_level_id >= 5 || !m_custom_data.empty())) {
                // Return to menu
                m_state_manager.pop_state();
            } else {
                // Restart current level
                if (!m_custom_data.empty()) {
                    m_world = std::make_unique<world::World>(m_custom_data);
                } else {
                    m_world = std::make_unique<world::World>(m_level_id);
                }
                m_action_button.reset();
                m_menu_shown = false;
            }
        });
    }

    void GameState::handle_input() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            m_state_manager.pop_state();
        }
        
        // Handle restart or level progression for keyboard
        if (m_world && (m_world->is_game_over() || m_world->is_level_complete())) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
                if (m_world->is_level_complete() && m_level_id > 0 && m_level_id < 5) {
                    // Advance to next level (only for standard levels)
                    m_level_id++;
                    m_world = std::make_unique<world::World>(m_level_id);
                    m_action_button.reset();
                    m_menu_shown = false;
                } else if (m_world->is_level_complete() && m_level_id >= 5) {
                    // Return to main menu after last level
                    m_state_manager.pop_state();
                } else if (m_world->is_level_complete() && !m_custom_data.empty()) {
                    // Custom level completed - return to editor
                    m_state_manager.pop_state();
                } else {
                    // Restart current level on game over
                    if (!m_custom_data.empty()) {
                        // Replay custom level
                        m_world = std::make_unique<world::World>(m_custom_data);
                    } else {
                        // Replay standard level
                        m_world = std::make_unique<world::World>(m_level_id);
                    }
                    m_action_button.reset();
                    m_menu_shown = false;
                }
            }
        }
    }

    void GameState::update(float dt) {
        if (m_world) {
            // Check if level just completed and save stars
            bool was_complete = m_world->is_level_complete();
            
            m_world->update(dt);
            
            // If level just became complete, save stars immediately
            if (!was_complete && m_world->is_level_complete()) {
                int coins = m_world->get_coins_collected();
                int total_coins = m_world->get_total_coins();
                int lives = m_world->get_player_lives();
                int stars = core::LevelProgress::instance().calculate_stars(coins, total_coins, lives);
                core::LevelProgress::instance().set_stars(m_level_id, stars);
                core::LevelProgress::instance().add_coins(coins);
                std::cout << "Level " << m_level_id << " completed with " << stars << " stars!" << std::endl;
            }
            
            // Create menu button when game over or level complete
            if ((m_world->is_game_over() || m_world->is_level_complete()) && !m_menu_shown) {
                create_menu_button(m_world->is_level_complete());
                m_menu_shown = true;
            }
            
            // Update button if menu is shown
            if (m_action_button) {
                m_action_button->update(m_mouse_pos, m_mouse_pressed);
            }
        }
    }

    void GameState::draw(core::GameWindow& window) {
        window.clear(sf::Color(100, 149, 237)); // Cornflower Blue Sky
        
        // Get mouse position for button interaction
        sf::Vector2i mouse_pix = sf::Mouse::getPosition(window.get_sf_window());
        m_mouse_pos = window.get_sf_window().mapPixelToCoords(mouse_pix, window.get_sf_window().getDefaultView());
        m_mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        
        if (m_world) {
            // Set camera view for world rendering
            window.get_sf_window().setView(m_world->get_camera());
            m_world->render(window);
            
            // Reset to default view for HUD
            window.get_sf_window().setView(window.get_sf_window().getDefaultView());
            
            // Draw lives as hearts
            auto& life_full_tex = core::ResourceManager::instance().get_texture("life_full");
            auto& life_empty_tex = core::ResourceManager::instance().get_texture("life_empty");
            
            int lives = m_world->get_player_lives();
            for (int i = 0; i < 3; ++i) {
                sf::Sprite heart_sprite(i < lives ? life_full_tex : life_empty_tex);
                heart_sprite.setScale({0.7f, 0.7f}); // Larger hearts
                heart_sprite.setPosition({10.0f + i * 40.0f, 10.0f}); // More spacing
                window.draw(heart_sprite);
            }
            
            // Draw coin counter in top-right
            auto& coin_tex = core::ResourceManager::instance().load_texture("coin_icon", "assets/gameplay/items/coin_gold.png");
            sf::Sprite coin_sprite(coin_tex);
            coin_sprite.setScale({0.4f, 0.4f});
            coin_sprite.setPosition({700.0f, 10.0f});
            window.draw(coin_sprite);
            
            if (m_status_text) {
                std::string coin_text = std::to_string(m_world->get_coins_collected()) + "/" + std::to_string(m_world->get_total_coins());
                m_status_text->setString(coin_text);
                m_status_text->setCharacterSize(24);
                m_status_text->setFillColor(sf::Color::Yellow);
                m_status_text->setOrigin({0.0f, 0.0f}); // Reset origin for coin text
                m_status_text->setPosition({740.0f, 10.0f});
                window.draw(*m_status_text);
            }
            
            // Show game over or victory message with panel
            if (m_status_text && m_panel_sprite) {
                if (m_world->is_game_over()) {
                    // Draw semi-transparent overlay
                    sf::RectangleShape overlay({1280.0f, 720.0f});
                    overlay.setFillColor(sf::Color(0, 0, 0, 150));
                    window.draw(overlay);
                    
                    // Draw panel
                    window.draw(*m_panel_sprite);
                    
                    // Draw divider
                    if (m_divider_sprite) {
                        window.draw(*m_divider_sprite);
                    }
                    
                    // Title
                    m_status_text->setString("GAME OVER");
                    m_status_text->setFillColor(sf::Color(255, 100, 100)); // Lighter red
                    m_status_text->setCharacterSize(48);
                    
                    sf::FloatRect text_bounds = m_status_text->getLocalBounds();
                    m_status_text->setOrigin({text_bounds.position.x + text_bounds.size.x / 2.0f, text_bounds.position.y + text_bounds.size.y / 2.0f});
                    m_status_text->setPosition({1280.0f / 2.0f, 280.0f});
                    window.draw(*m_status_text);
                    
                    // Keyboard hint
                    sf::Text hint_text(m_status_text->getFont());
                    hint_text.setCharacterSize(16);
                    hint_text.setFillColor(sf::Color(200, 200, 200));
                    hint_text.setString("Press R or click the button below");
                    
                    sf::FloatRect hint_bounds = hint_text.getLocalBounds();
                    hint_text.setOrigin({hint_bounds.position.x + hint_bounds.size.x / 2.0f, hint_bounds.position.y + hint_bounds.size.y / 2.0f});
                    hint_text.setPosition({1280.0f / 2.0f, 480.0f});
                    window.draw(hint_text);
                    
                    // Draw button
                    if (m_action_button) {
                        m_action_button->render(window);
                    }
                    
                } else if (m_world->is_level_complete()) {
                    // Draw semi-transparent overlay
                    sf::RectangleShape overlay({1280.0f, 720.0f});
                    overlay.setFillColor(sf::Color(0, 0, 0, 150));
                    window.draw(overlay);
                    
                    // Draw panel
                    window.draw(*m_panel_sprite);
                    
                    // Draw divider
                    if (m_divider_sprite) {
                        window.draw(*m_divider_sprite);
                    }
                    
                    // Title based on game completion
                    if (m_level_id < 5) {
                        m_status_text->setString("LEVEL COMPLETE!");
                        m_status_text->setFillColor(sf::Color(100, 255, 100)); // Light green
                    } else {
                        m_status_text->setString("GAME COMPLETE!");
                        m_status_text->setFillColor(sf::Color(255, 255, 100)); // Yellow
                    }
                    m_status_text->setCharacterSize(48);
                    
                    sf::FloatRect text_bounds = m_status_text->getLocalBounds();
                    m_status_text->setOrigin({text_bounds.position.x + text_bounds.size.x / 2.0f, text_bounds.position.y + text_bounds.size.y / 2.0f});
                    m_status_text->setPosition({1280.0f / 2.0f, 260.0f});
                    window.draw(*m_status_text);
                    
                    // Draw stars
                    int coins = m_world->get_coins_collected();
                    int total_coins = m_world->get_total_coins();
                    int player_lives = m_world->get_player_lives();
                    int stars = core::LevelProgress::instance().calculate_stars(coins, total_coins, player_lives);
                    
                    auto& star_filled = core::ResourceManager::instance().get_texture("star_filled");
                    auto& star_empty = core::ResourceManager::instance().get_texture("star_empty");
                    
                    float star_scale = 0.7f;
                    float star_spacing = 50.0f;
                    float stars_start_x = 1280.0f / 2.0f - star_spacing;
                    float star_y = 340.0f;
                    
                    for (int i = 0; i < 3; ++i) {
                        sf::Sprite star_sprite(i < stars ? star_filled : star_empty);
                        star_sprite.setScale({star_scale, star_scale});
                        sf::FloatRect star_bounds = star_sprite.getLocalBounds();
                        star_sprite.setOrigin({star_bounds.size.x / 2.0f, star_bounds.size.y / 2.0f});
                        star_sprite.setPosition({stars_start_x + i * star_spacing, star_y});
                        window.draw(star_sprite);
                    }
                    
                    // Keyboard hint
                    sf::Text hint_text(m_status_text->getFont());
                    hint_text.setCharacterSize(16);
                    hint_text.setFillColor(sf::Color(200, 200, 200));
                    hint_text.setString("Press R or click the button below");
                    
                    sf::FloatRect hint_bounds = hint_text.getLocalBounds();
                    hint_text.setOrigin({hint_bounds.position.x + hint_bounds.size.x / 2.0f, hint_bounds.position.y + hint_bounds.size.y / 2.0f});
                    hint_text.setPosition({1280.0f / 2.0f, 480.0f});
                    window.draw(hint_text);
                    
                    // Draw button
                    if (m_action_button) {
                        m_action_button->render(window);
                    }
                }
            }
        }
    }

} // namespace states

