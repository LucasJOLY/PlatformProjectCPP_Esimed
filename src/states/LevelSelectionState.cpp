#include "LevelSelectionState.hpp"
#include "GameState.hpp"
#include "../core/ResourceManager.hpp"
#include "../core/GameWindow.hpp"
#include "../core/LevelProgress.hpp"
#include <iostream>

namespace states {

    LevelSelectionState::LevelSelectionState(StateManager& state_manager)
        : m_state_manager(state_manager),
          m_background(core::ResourceManager::instance().get_texture("menu_bg")), // Reuse menu background
          m_title(core::ResourceManager::instance().get_font("cosmic_font"))
    {}

    void LevelSelectionState::init() {
        // Setup Background
        const auto& bg_tex = core::ResourceManager::instance().get_texture("menu_bg");
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
        
        // Load Click Sound
        if (!rm.has_sound_buffer("click_sound")) rm.load_sound_buffer("click_sound", "assets/menu/click.ogg");

        // Create Level Buttons
        auto& font = core::ResourceManager::instance().get_font("cosmic_font");
        float start_x = 1280.0f / 2.0f - 250.0f; // Centered grid
        float start_y = 250.0f;

        for (int i = 1; i <= 5; ++i) {
            bool unlocked = core::LevelProgress::instance().is_unlocked(i);
            
            // For locked levels, we don't show the number, but we handle that in draw or here?
            // User said "lock icon in the middle". 
            // If I put text " " (empty), UIButton won't draw text.
            std::string label = unlocked ? std::to_string(i) : ""; 
            
            float x = start_x + ((i - 1) % 3) * (100.0f + 50.0f); // 3 columns
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
            // Locked buttons have no callback
            
            m_buttons.push_back(std::move(btn));
        }

        // Back Button
        auto back_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f{50.0f, 650.0f}, 
            sf::Vector2f{200.f, 50.f}, 
            "BACK", 
            "button_cosmic", 
            "click_sound", 
            font
        );
        back_btn->set_callback([this]() {
            m_state_manager.pop_state();
        });
        m_buttons.push_back(std::move(back_btn));
    }

    void LevelSelectionState::handle_input() {
        auto mouse_pos_i = sf::Mouse::getPosition(m_state_manager.get_window().get_sf_window());
        sf::Vector2f mouse_pos(static_cast<float>(mouse_pos_i.x), static_cast<float>(mouse_pos_i.y));
        bool mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        for (auto& btn : m_buttons) {
            btn->update(mouse_pos, mouse_pressed);
        }
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

        // Draw buttons and overlays
        // We iterate up to 5 for levels, the last button is "Back"
        for (size_t i = 0; i < m_buttons.size(); ++i) {
            m_buttons[i]->render(window);
            
            // If it's a level button (index 0 to 4)
            if (i < 5) {
                int level_id = static_cast<int>(i) + 1;
                bool unlocked = core::LevelProgress::instance().is_unlocked(level_id);
                sf::Vector2f pos = m_buttons[i]->get_position();
                sf::Vector2f size = m_buttons[i]->get_size();
                sf::Vector2f center = {pos.x + size.x / 2.0f, pos.y + size.y / 2.0f};

                if (!unlocked) {
                    // Draw lock icon in center
                    sf::Sprite lock_sprite(lock_tex);
                    // Scale lock to fit nicely (e.g. 40x40)
                    sf::Vector2u lock_size = lock_tex.getSize();
                    lock_sprite.setScale({40.0f / lock_size.x, 40.0f / lock_size.y});
                    lock_sprite.setOrigin({lock_size.x / 2.0f, lock_size.y / 2.0f});
                    lock_sprite.setPosition(center);
                    window.draw(lock_sprite);
                } else {
                    // Draw stars above
                    int stars = core::LevelProgress::instance().get_stars(level_id);
                    // Draw 3 stars, filled or empty
                    // Position: above button, slightly overlapping
                    float star_y = pos.y - 10.0f; // Slightly above top edge? Or overlapping top edge?
                    // User said "un peu superposé" (slightly overlapping).
                    // Let's put them at top of button.
                    
                    float star_spacing = 25.0f;
                    float star_start_x = center.x - star_spacing; // Center the 3 stars
                    
                    for (int s = 0; s < 3; ++s) {
                        sf::Sprite star_sprite(s < stars ? star_full_tex : star_empty_tex);
                        star_sprite.setScale({0.5f, 0.5f}); // Small stars
                        // Adjust origin to center
                        sf::Vector2u s_size = star_sprite.getTexture().getSize();
                        star_sprite.setOrigin({s_size.x / 2.0f, s_size.y / 2.0f});
                        
                        // Arc arrangement or straight line? Straight line is easier.
                        star_sprite.setPosition({star_start_x + s * star_spacing, star_y});
                        window.draw(star_sprite);
                    }
                }
            }
        }
    }

} // namespace states
