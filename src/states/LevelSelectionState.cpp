#include "LevelSelectionState.hpp"
#include "GameState.hpp"
#include "../core/ResourceManager.hpp"
#include "../core/GameWindow.hpp"
#include "../core/LevelProgress.hpp"
#include "../core/CustomLevelManager.hpp"
#include <iostream>

namespace states {

    LevelSelectionState::LevelSelectionState(StateManager& state_manager)
        : m_state_manager(state_manager),
          m_background(core::ResourceManager::instance().load_texture("menu_bg_clean", "assets/menu/background-menu-clean.png")),
          m_title(core::ResourceManager::instance().get_font("cosmic_font"))
    {}

    void LevelSelectionState::init() {
        // Setup Background
        const auto& bg_tex = core::ResourceManager::instance().get_texture("menu_bg_clean");
        sf::Vector2u bg_size = bg_tex.getSize();
        m_background.setScale({1280.0f / bg_size.x, 720.0f / bg_size.y});

        // Setup Title
        m_title.setString("Select Level");
        m_title.setCharacterSize(80);
        m_title.setFillColor(sf::Color(100, 200, 255));
        m_title.setOutlineColor(sf::Color(0, 50, 100));
        m_title.setOutlineThickness(4.0f);
        
        sf::FloatRect title_bounds = m_title.getLocalBounds();
        m_title.setOrigin({title_bounds.position.x + title_bounds.size.x / 2.0f,
                           title_bounds.position.y + title_bounds.size.y / 2.0f});
        m_title.setPosition({1280.0f / 2.0f, 100.0f});

        // Load UI Assets
        auto& rm = core::ResourceManager::instance();
        rm.load_texture("button_round", "assets/Pack_to_pick/UI/PNG/Blue/Default/button_round_depth_flat.png");
        rm.load_texture("lock_icon", "assets/Pack_to_pick/Game/Sprites/Tiles/Default/lock_blue.png");
        rm.load_texture("star_full", "assets/Pack_to_pick/UI/PNG/Yellow/Default/star.png");
        rm.load_texture("star_empty", "assets/Pack_to_pick/UI/PNG/Grey/Default/star.png");
        rm.load_texture("button_blue_rect", "assets/Pack_to_pick/UI/PNG/Blue/Default/button_rectangle_depth_flat.png");
        
        // Load Click Sound
        if (!rm.has_sound_buffer("click_sound")) rm.load_sound_buffer("click_sound", "assets/menu/click.ogg");

        auto& font = rm.get_font("cosmic_font");

        // Back Button
        m_back_button = std::make_unique<ui::UIButton>(
            sf::Vector2f{50.0f, 650.0f}, 
            sf::Vector2f{200.f, 50.f}, 
            "BACK", 
            "button_cosmic", 
            "click_sound", 
            font
        );
        m_back_button->set_callback([this]() {
            m_state_manager.pop_state();
        });

        // Toggle Button (above back button)
        m_toggle_button = std::make_unique<ui::UIButton>(
            sf::Vector2f{50.0f, 580.0f}, 
            sf::Vector2f{200.f, 50.f}, 
            "CUSTOM", 
            "button_blue_rect", 
            "click_sound", 
            font
        );
        m_toggle_button->set_callback([this]() {
            switch_view();
        });

        // Start with standard levels
        m_showing_custom = false;
        create_standard_level_buttons();
    }

    void LevelSelectionState::switch_view() {
        m_showing_custom = !m_showing_custom;
        m_level_buttons.clear();
        
        if (m_showing_custom) {
            m_title.setString("Niveaux Custom");
            m_toggle_button->set_label("STANDARD");
            create_custom_level_buttons();
        } else {
            m_title.setString("Select Level");
            m_toggle_button->set_label("CUSTOM");
            create_standard_level_buttons();
        }
    }

    void LevelSelectionState::create_standard_level_buttons() {
        m_level_buttons.clear();
        
        auto& font = core::ResourceManager::instance().get_font("cosmic_font");
        float start_x = 1280.0f / 2.0f - 250.0f;
        float start_y = 250.0f;

        for (int i = 1; i <= 5; ++i) {
            bool unlocked = core::LevelProgress::instance().is_unlocked(i);
            std::string label = unlocked ? std::to_string(i) : ""; 
            
            float x = start_x + ((i - 1) % 3) * (100.0f + 50.0f);
            float y = start_y + ((i - 1) / 3) * (100.0f + 50.0f);

            auto btn = std::make_unique<ui::UIButton>(
                sf::Vector2f{x, y}, 
                sf::Vector2f{100.f, 100.f}, 
                label, 
                "button_round", 
                "click_sound", 
                font
            );
            
            int level = i;
            if (unlocked) {
                btn->set_callback([this, level]() {
                    std::cout << "Level " << level << " selected" << std::endl;
                    m_state_manager.push_state(std::make_unique<GameState>(m_state_manager, level));
                });
            }
            
            m_level_buttons.push_back(std::move(btn));
        }
    }

    void LevelSelectionState::create_custom_level_buttons() {
        m_level_buttons.clear();
        
        auto& rm = core::ResourceManager::instance();
        auto& font = rm.get_font("cosmic_font");
        
        const auto& levels = core::CustomLevelManager::instance().get_all_levels();
        if (levels.empty()) return;
        
        float start_x = 1280.0f / 2.0f - 200.0f;
        float start_y = 220.0f;
        float gap_y = 60.0f;
        
        for (size_t i = 0; i < levels.size(); ++i) {
            const auto& level = levels[i];
            float y = start_y + i * gap_y;
            
            auto btn = std::make_unique<ui::UIButton>(
                sf::Vector2f{start_x, y}, 
                sf::Vector2f{400.f, 50.f}, 
                level.name, 
                "button_blue_rect", 
                "click_sound", 
                font
            );
            
            std::string level_data = level.data;
            btn->set_callback([this, level_data]() {
                m_state_manager.push_state(std::make_unique<GameState>(m_state_manager, level_data, false));
            });
            
            m_level_buttons.push_back(std::move(btn));
        }
    }

    void LevelSelectionState::handle_input() {
        auto mouse_pos_i = sf::Mouse::getPosition(m_state_manager.get_window().get_sf_window());
        sf::Vector2f mouse_pos(static_cast<float>(mouse_pos_i.x), static_cast<float>(mouse_pos_i.y));
        bool mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        for (auto& btn : m_level_buttons) {
            btn->update(mouse_pos, mouse_pressed);
        }
        
        if (m_back_button) m_back_button->update(mouse_pos, mouse_pressed);
        if (m_toggle_button) m_toggle_button->update(mouse_pos, mouse_pressed);
    }

    void LevelSelectionState::update(float dt) {
        (void)dt;
    }

    void LevelSelectionState::draw(core::GameWindow& window) {
        window.draw(m_background);
        window.draw(m_title);
        
        auto& rm = core::ResourceManager::instance();
        const auto& lock_tex = rm.get_texture("lock_icon");
        const auto& star_full_tex = rm.get_texture("star_full");
        const auto& star_empty_tex = rm.get_texture("star_empty");

        // Draw level buttons
        for (size_t i = 0; i < m_level_buttons.size(); ++i) {
            m_level_buttons[i]->render(window);
            
            // For standard levels, show lock icons and stars
            if (!m_showing_custom && i < 5) {
                int level_id = static_cast<int>(i) + 1;
                bool unlocked = core::LevelProgress::instance().is_unlocked(level_id);
                sf::Vector2f pos = m_level_buttons[i]->get_position();
                sf::Vector2f size = m_level_buttons[i]->get_size();
                sf::Vector2f center = {pos.x + size.x / 2.0f, pos.y + size.y / 2.0f};
                
                if (!unlocked) {
                    // Draw lock icon
                    sf::Sprite lock_sprite(lock_tex);
                    lock_sprite.setScale({0.8f, 0.8f});
                    sf::Vector2u l_size = lock_tex.getSize();
                    lock_sprite.setOrigin({l_size.x / 2.0f, l_size.y / 2.0f});
                    lock_sprite.setPosition(center);
                    window.draw(lock_sprite);
                } else {
                    // Draw stars
                    int stars = core::LevelProgress::instance().get_stars(level_id);
                    float star_y = pos.y - 20.0f;
                    float star_spacing = 20.0f;
                    float star_start_x = center.x - star_spacing;
                    
                    for (int s = 0; s < 3; ++s) {
                        const sf::Texture& star_tex = (s < stars) ? star_full_tex : star_empty_tex;
                        sf::Sprite star_sprite(star_tex);
                        star_sprite.setScale({0.5f, 0.5f});
                        sf::Vector2u s_size = star_sprite.getTexture().getSize();
                        star_sprite.setOrigin({s_size.x / 2.0f, s_size.y / 2.0f});
                        star_sprite.setPosition({star_start_x + s * star_spacing, star_y});
                        window.draw(star_sprite);
                    }
                }
            }
        }
        
        // Show "no custom levels" message if in custom mode and empty
        if (m_showing_custom && m_level_buttons.empty()) {
            auto& font = rm.get_font("cosmic_font");
            sf::Text empty_text(font, "Aucun niveau custom!", 28);
            empty_text.setFillColor(sf::Color(200, 200, 200));
            sf::FloatRect bounds = empty_text.getLocalBounds();
            empty_text.setOrigin({bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
            empty_text.setPosition({1280.0f / 2.0f, 350.0f});
            window.draw(empty_text);
        }
        
        // Draw back and toggle buttons
        if (m_back_button) m_back_button->render(window);
        if (m_toggle_button) m_toggle_button->render(window);
    }

} // namespace states
