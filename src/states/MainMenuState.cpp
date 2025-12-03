#include "MainMenuState.hpp"
#include "StateManager.hpp"
#include "LevelSelectionState.hpp"
#include "SkinSelectionState.hpp"
#include "../core/ResourceManager.hpp"
#include "../core/GameWindow.hpp"
#include <iostream>

namespace states {

    MainMenuState::MainMenuState(StateManager& state_manager)
        : m_state_manager(state_manager),
          m_background(core::ResourceManager::instance().load_texture("menu_bg", "assets/menu/background-menu.png")),
          m_title(core::ResourceManager::instance().load_font("cosmic_font", "assets/menu/font_cosmic.ttf"))
    {}
    void MainMenuState::init() {
        // Background texture already loaded in constructor, just need to scale it
        const auto& bg_tex = core::ResourceManager::instance().get_texture("menu_bg");
        
        // Scale background to fit window (assuming 1280x720)
        sf::Vector2u bg_size = bg_tex.getSize();
        m_background.setScale({1280.0f / bg_size.x, 720.0f / bg_size.y});

        // Get Font (already loaded)
        auto& font = core::ResourceManager::instance().get_font("cosmic_font");
        
        // Load Button Texture (Cosmic)
        (void)core::ResourceManager::instance().load_texture("button_cosmic", "assets/menu/button_cosmic.png");

        // Load Click Sound
        (void)core::ResourceManager::instance().load_sound_buffer("click_sound", "assets/menu/click.ogg");

        // Setup Title
        m_title.setFont(font);
        m_title.setString("Cosmic Quest");
        m_title.setCharacterSize(100); 
        m_title.setFillColor(sf::Color(100, 200, 255)); // Light Blue
        m_title.setOutlineColor(sf::Color(0, 50, 100)); // Dark Blue outline
        m_title.setOutlineThickness(5.0f);
        
        // Center Title
        sf::FloatRect title_bounds = m_title.getLocalBounds();
        m_title.setOrigin({title_bounds.position.x + title_bounds.size.x / 2.0f,
                           title_bounds.position.y + title_bounds.size.y / 2.0f});
        m_title.setPosition({1280.0f / 2.0f, 130.0f}); 

        // Create Buttons
        float center_x = 1280.0f / 2.0f - 175.0f; // Center - half width (width is 350 now)
        float start_y = 350.0f;
        float gap_y = 120.0f; 

        // Use the cosmic texture and sound for all buttons
        auto play_btn = std::make_unique<ui::UIButton>(sf::Vector2f{center_x, start_y}, sf::Vector2f{350.f, 100.f}, "PLAY", "button_cosmic", "click_sound", font);
        play_btn->set_callback([this]() { 
            m_state_manager.push_state(std::make_unique<LevelSelectionState>(m_state_manager));
        });
        m_buttons.push_back(std::move(play_btn));

        auto skins_btn = std::make_unique<ui::UIButton>(sf::Vector2f{center_x, start_y + gap_y}, sf::Vector2f{350.f, 100.f}, "SKINS", "button_cosmic", "click_sound", font);
        skins_btn->set_callback([this]() {
             m_state_manager.change_state(std::make_unique<SkinSelectionState>(m_state_manager));
        });
        m_buttons.push_back(std::move(skins_btn));

        auto editor_btn = std::make_unique<ui::UIButton>(sf::Vector2f{center_x, start_y + gap_y * 2}, sf::Vector2f{350.f, 100.f}, "EDITOR", "button_cosmic", "click_sound", font);
        editor_btn->set_callback([]() { std::cout << "Editor Clicked!" << std::endl; });
        m_buttons.push_back(std::move(editor_btn));


    }

    void MainMenuState::handle_input() {
        // We need mouse position from window, but StateManager handles events.
        // We can get mouse pos directly from sf::Mouse relative to window
        auto mouse_pos_i = sf::Mouse::getPosition(m_state_manager.get_window().get_sf_window());
        sf::Vector2f mouse_pos(static_cast<float>(mouse_pos_i.x), static_cast<float>(mouse_pos_i.y));
        bool mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        for (auto& btn : m_buttons) {
            btn->update(mouse_pos, mouse_pressed);
        }
    }

    void MainMenuState::update(float dt) {
        (void)dt; // Unused for now
    }

    void MainMenuState::draw(core::GameWindow& window) {
        window.draw(m_background);
        window.draw(m_title);
        for (auto& btn : m_buttons) {
            btn->render(window);
        }
    }

} // namespace states
