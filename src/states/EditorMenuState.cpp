#include "EditorMenuState.hpp"
#include "LevelEditorState.hpp"
#include "MainMenuState.hpp"
#include "../core/ResourceManager.hpp"
#include "../core/GameWindow.hpp"
#include "../core/CustomLevelManager.hpp"
#include <iostream>

namespace states {

    EditorMenuState::EditorMenuState(StateManager& state_manager)
        : m_state_manager(state_manager),
          m_background(core::ResourceManager::instance().get_texture("menu_bg")),
          m_title(core::ResourceManager::instance().get_font("cosmic_font"))
    {}

    void EditorMenuState::init() {
        // Setup Background
        const auto& bg_tex = core::ResourceManager::instance().get_texture("menu_bg");
        sf::Vector2u bg_size = bg_tex.getSize();
        m_background.setScale({1280.0f / bg_size.x, 720.0f / bg_size.y});

        // Setup Title
        m_title.setString("Level Editor");
        m_title.setCharacterSize(70);
        m_title.setFillColor(sf::Color(100, 200, 255));
        m_title.setOutlineColor(sf::Color(0, 50, 100));
        m_title.setOutlineThickness(4.0f);
        
        sf::FloatRect title_bounds = m_title.getLocalBounds();
        m_title.setOrigin({title_bounds.position.x + title_bounds.size.x / 2.0f,
                           title_bounds.position.y + title_bounds.size.y / 2.0f});
        m_title.setPosition({1280.0f / 2.0f, 80.0f});

        // Load UI Assets
        auto& rm = core::ResourceManager::instance();
        rm.load_texture("button_blue_rect", "assets/Pack_to_pick/UI/PNG/Blue/Default/button_rectangle_depth_flat.png");
        rm.load_texture("button_green_rect", "assets/Pack_to_pick/UI/PNG/Green/Default/button_rectangle_depth_flat.png");
        rm.load_texture("button_red_rect", "assets/Pack_to_pick/UI/PNG/Red/Default/button_rectangle_depth_flat.png");
        
        // Load tap sound
        if (!rm.has_sound_buffer("tap_sound")) {
            rm.load_sound_buffer("tap_sound", "assets/Pack_to_pick/UI/Sounds/tap-a.ogg");
        }
        
        auto& font = rm.get_font("cosmic_font");

        // Create "New Level" Button
        auto new_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{1280.0f / 2.0f - 150.0f, 140.0f}, 
            sf::Vector2f{300.f, 60.f}, 
            "NOUVEAU NIVEAU", 
            "button_green_rect", 
            "tap_sound", 
            font
        );
        new_btn->set_callback([this]() {
            m_state_manager.push_state(std::make_unique<LevelEditorState>(m_state_manager, -1)); // -1 = new level
        });
        m_buttons.push_back(std::move(new_btn));

        // Back Button
        auto back_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{50.0f, 650.0f}, 
            sf::Vector2f{200.f, 50.f}, 
            "RETOUR", 
            "button_cosmic", 
            "tap_sound", 
            font
        );
        back_btn->set_callback([this]() {
            m_state_manager.pop_state();
        });
        m_buttons.push_back(std::move(back_btn));

        // Load level list
        refresh_level_list();
    }

    void EditorMenuState::refresh_level_list() {
        m_level_buttons.clear();
        
        auto& rm = core::ResourceManager::instance();
        auto& font = rm.get_font("cosmic_font");
        
        const auto& levels = core::CustomLevelManager::instance().get_all_levels();
        
        float start_y = 220.0f;
        float gap_y = 70.0f;
        
        for (size_t i = 0; i < levels.size(); ++i) {
            const auto& level = levels[i];
            float y = start_y + i * gap_y - m_scroll_offset;
            
            // Level name button (to edit)
            auto name_btn = std::make_unique<ui::UIButton>(
                sf::Vector2f{200.0f, y}, 
                sf::Vector2f{500.f, 55.f}, 
                level.name, 
                "button_blue_rect", 
                "tap_sound", 
                font
            );
            int level_id = level.id;
            name_btn->set_callback([this, level_id]() {
                m_state_manager.push_state(std::make_unique<LevelEditorState>(m_state_manager, level_id));
            });
            m_level_buttons.push_back(std::move(name_btn));
            
            // Delete button
            auto del_btn = std::make_unique<ui::UIButton>(
                sf::Vector2f{720.0f, y}, 
                sf::Vector2f{120.f, 55.f}, 
                "SUPPR", 
                "button_red_rect", 
                "tap_sound", 
                font
            );
            del_btn->set_callback([this, level_id]() {
                core::CustomLevelManager::instance().delete_level(level_id);
                refresh_level_list();
            });
            m_level_buttons.push_back(std::move(del_btn));
        }
        
        // Calculate max scroll
        float total_height = levels.size() * gap_y;
        float visible_height = 400.0f; // From y=220 to y=620
        m_max_scroll = std::max(0.0f, total_height - visible_height);
    }

    void EditorMenuState::handle_input() {
        auto mouse_pos_i = sf::Mouse::getPosition(m_state_manager.get_window().get_sf_window());
        sf::Vector2f mouse_pos(static_cast<float>(mouse_pos_i.x), static_cast<float>(mouse_pos_i.y));
        bool mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        for (auto& btn : m_buttons) {
            btn->update(mouse_pos, mouse_pressed);
        }
        
        for (auto& btn : m_level_buttons) {
            btn->update(mouse_pos, mouse_pressed);
        }
    }

    void EditorMenuState::update(float dt) {
        (void)dt;
        // Refresh level list in case we returned from editor
        static size_t last_count = 0;
        size_t current_count = core::CustomLevelManager::instance().get_all_levels().size();
        if (current_count != last_count) {
            last_count = current_count;
            refresh_level_list();
        }
    }

    void EditorMenuState::draw(core::GameWindow& window) {
        // Solid blue background
        window.clear(sf::Color(50, 120, 180));
        window.draw(m_title);
        
        for (auto& btn : m_buttons) {
            btn->render(window);
        }
        
        // Draw level buttons (with scroll clipping would be ideal, but render all for simplicity)
        for (auto& btn : m_level_buttons) {
            btn->render(window);
        }
        
        // Draw "no levels" text if empty
        if (m_level_buttons.empty()) {
            auto& font = core::ResourceManager::instance().get_font("cosmic_font");
            sf::Text empty_text(font, "Aucun niveau custom. Cliquez sur NOUVEAU NIVEAU!", 24);
            empty_text.setFillColor(sf::Color(200, 200, 200));
            sf::FloatRect bounds = empty_text.getLocalBounds();
            empty_text.setOrigin({bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
            empty_text.setPosition({1280.0f / 2.0f, 350.0f});
            window.draw(empty_text);
        }
    }

} // namespace states
