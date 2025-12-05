#include "SkinSelectionState.hpp"
#include "../core/ResourceManager.hpp"
#include "../core/LevelProgress.hpp"
#include "../core/SkinManager.hpp"
#include "MainMenuState.hpp"
#include "../core/GameWindow.hpp"
#include <iostream>

namespace states {

    SkinSelectionState::SkinSelectionState(StateManager& state_manager)
        : m_state_manager(state_manager),
          m_title(core::ResourceManager::instance().get_font("main_font")),
          m_wallet_text(core::ResourceManager::instance().get_font("main_font")) {
    }

    void SkinSelectionState::init() {
        auto& rm = core::ResourceManager::instance();
        
        // Ensure assets are loaded
        if (!rm.has_texture("menu_bg_clean")) rm.load_texture("menu_bg_clean", "assets/menu/background-menu-clean.png");
        if (!rm.has_font("cosmic_font")) rm.load_font("cosmic_font", "assets/menu/font_cosmic.ttf");
        if (!rm.has_texture("button_cosmic")) rm.load_texture("button_cosmic", "assets/menu/button_cosmic.png");
        if (!rm.has_texture("hud_coin")) rm.load_texture("hud_coin", "assets/Pack_to_pick/Game/Sprites/Tiles/Default/hud_coin.png");
        
        // Load UI assets for improved skin selection
        if (!rm.has_texture("button_round_blue")) rm.load_texture("button_round_blue", "assets/Pack_to_pick/UI/PNG/Blue/Default/button_round_depth_flat.png");
        if (!rm.has_texture("check_selected")) rm.load_texture("check_selected", "assets/Pack_to_pick/UI/PNG/Green/Default/icon_checkmark.png");
        if (!rm.has_texture("divider")) rm.load_texture("divider", "assets/Pack_to_pick/UI/PNG/Extra/Default/divider.png");
        if (!rm.has_texture("lock_icon")) rm.load_texture("lock_icon", "assets/Pack_to_pick/Game/Sprites/Tiles/Default/lock_blue.png");
        if (!rm.has_texture("star_full")) rm.load_texture("star_full", "assets/Pack_to_pick/UI/PNG/Yellow/Default/star.png");
        
        // Load Click Sound
        if (!rm.has_sound_buffer("click_sound")) rm.load_sound_buffer("click_sound", "assets/menu/click.ogg");
        
        // Background
        m_background.emplace(rm.get_texture("menu_bg_clean"));
        sf::Vector2u bg_size = m_background->getTexture().getSize();
        m_background->setScale({1280.0f / bg_size.x, 720.0f / bg_size.y});
        
        // Title
        m_title.setFont(rm.get_font("cosmic_font"));
        m_title.setString("SKINS");
        m_title.setCharacterSize(80);
        m_title.setFillColor(sf::Color(100, 200, 255));
        m_title.setOutlineColor(sf::Color(0, 50, 100));
        m_title.setOutlineThickness(4.0f);
        
        sf::FloatRect title_bounds = m_title.getLocalBounds();
        m_title.setOrigin({title_bounds.position.x + title_bounds.size.x / 2.0f,
                           title_bounds.position.y + title_bounds.size.y / 2.0f});
        m_title.setPosition({1280.0f / 2.0f, 80.0f});
        
        // Create decorative dividers
        const auto& divider_tex = rm.get_texture("divider");
        sf::Vector2u div_size = divider_tex.getSize();
        
        m_divider_top.emplace(divider_tex);
        m_divider_top->setScale({600.0f / div_size.x, 1.0f});
        m_divider_top->setOrigin({div_size.x / 2.0f, div_size.y / 2.0f});
        m_divider_top->setPosition({1280.0f / 2.0f, 140.0f});
        
        m_divider_bottom.emplace(divider_tex);
        m_divider_bottom->setScale({600.0f / div_size.x, 1.0f});
        m_divider_bottom->setOrigin({div_size.x / 2.0f, div_size.y / 2.0f});
        m_divider_bottom->setPosition({1280.0f / 2.0f, 580.0f});
        
        create_ui();
    }

    void SkinSelectionState::create_ui() {
        m_buttons.clear();
        m_skin_labels.clear();
        m_skin_previews.clear();
        m_skin_circles.clear();
        m_selected_checks.clear();
        m_is_selected.clear();
        
        auto& rm = core::ResourceManager::instance();
        auto& progress = core::LevelProgress::instance();
        auto& skin_manager = core::SkinManager::instance();
        
        // Wallet Display
        m_wallet_coin_sprite.emplace(rm.get_texture("hud_coin"));
        m_wallet_coin_sprite->setScale({0.6f, 0.6f});
        m_wallet_coin_sprite->setPosition({1050.0f, 30.0f});
        
        m_wallet_text.setFont(rm.get_font("cosmic_font"));
        m_wallet_text.setString(std::to_string(progress.get_wallet()));
        m_wallet_text.setCharacterSize(40);
        m_wallet_text.setFillColor(sf::Color::Yellow);
        m_wallet_text.setOutlineColor(sf::Color::Black);
        m_wallet_text.setOutlineThickness(2.0f);
        m_wallet_text.setPosition({1110.0f, 25.0f});
        
        const auto& skins = skin_manager.get_all_skins();
        float start_x = 280.0f;
        float start_y = 280.0f;
        float padding_x = 230.0f;
        float padding_y = 200.0f;
        int cols = 3;
        
        const auto& circle_tex = rm.get_texture("button_round_blue");
        const auto& check_tex = rm.get_texture("check_selected");
        sf::Vector2u circle_tex_size = circle_tex.getSize();
        
        for (size_t i = 0; i < skins.size(); ++i) {
            const auto& skin = skins[i];
            float x = start_x + (i % cols) * padding_x;
            float y = start_y + (i / cols) * padding_y;
            
            bool is_unlocked = progress.is_skin_unlocked(skin.id);
            bool is_selected = (progress.get_selected_skin() == skin.id);
            m_is_selected.push_back(is_selected);
            
            // Blue circle background (like level selection)
            sf::Sprite circle_sprite(circle_tex);
            float circle_scale = 100.0f / circle_tex_size.x;
            circle_sprite.setScale({circle_scale, circle_scale});
            circle_sprite.setOrigin({circle_tex_size.x / 2.0f, circle_tex_size.y / 2.0f});
            circle_sprite.setPosition({x, y});
            
            // Adjust color based on state
            if (is_selected) {
                circle_sprite.setColor(sf::Color(100, 255, 100)); // Green tint for selected
            } else if (!is_unlocked) {
                circle_sprite.setColor(sf::Color(150, 150, 150)); // Grey for locked
            }
            m_skin_circles.push_back(circle_sprite);
            
            // Helmet preview inside the circle
            std::string helmet_path = "assets/Pack_to_pick/Game/Sprites/Tiles/Default/hud_player_helmet_" + skin.id + ".png";
            if (!rm.has_texture("helmet_" + skin.id)) {
                rm.load_texture("helmet_" + skin.id, helmet_path);
            }
            
            sf::Sprite preview(rm.get_texture("helmet_" + skin.id));
            sf::Vector2u tex_size = preview.getTexture().getSize();
            float preview_scale = 60.0f / std::max(tex_size.x, tex_size.y);
            preview.setScale({preview_scale, preview_scale});
            preview.setOrigin({static_cast<float>(tex_size.x) / 2.0f, static_cast<float>(tex_size.y) / 2.0f});
            preview.setPosition({x, y});
            
            if (!is_unlocked) {
                preview.setColor(sf::Color(100, 100, 100, 200)); // Darken locked skins
            }
            m_skin_previews.push_back(preview);
            
            // Checkmark for selected skin
            sf::Sprite check_sprite(check_tex);
            sf::Vector2u check_size = check_tex.getSize();
            check_sprite.setScale({0.6f, 0.6f});
            check_sprite.setOrigin({check_size.x / 2.0f, check_size.y / 2.0f});
            check_sprite.setPosition({x + 35.0f, y - 35.0f}); // Top-right of circle
            m_selected_checks.push_back(check_sprite);
            
            // Skin name label
            m_skin_labels.emplace_back(rm.get_font("cosmic_font"));
            sf::Text& label = m_skin_labels.back();
            label.setString(skin.name);
            label.setCharacterSize(22);
            label.setFillColor(sf::Color::White);
            label.setOutlineColor(sf::Color::Black);
            label.setOutlineThickness(1.5f);
            sf::FloatRect label_bounds = label.getLocalBounds();
            label.setOrigin({label_bounds.position.x + label_bounds.size.x / 2.0f, label_bounds.position.y});
            label.setPosition({x, y + 60.0f});
            
            // Action button below the circle
            std::string btn_text;
            std::string btn_texture = "button_cosmic";
            
            if (is_selected) {
                btn_text = "EQUIPPED";
            } else if (is_unlocked) {
                btn_text = "SELECT";
            } else {
                btn_text = std::to_string(skin.cost);
            }
            
            auto btn = std::make_unique<ui::UIButton>(
                sf::Vector2f(x - 65.0f, y + 85.0f),
                sf::Vector2f(130.0f, 45.0f),
                btn_text,
                btn_texture,
                "click_sound",
                rm.get_font("cosmic_font")
            );
            
            // Add coin icon for locked skins
            if (!is_unlocked && !is_selected) {
                btn->set_icon(rm.get_texture("hud_coin"));
            }
            
            // Set button callbacks
            if (is_selected) {
                // Already equipped, no action
            } else if (is_unlocked) {
                btn->set_callback([this, skin_id = skin.id]() {
                    core::LevelProgress::instance().select_skin(skin_id);
                    create_ui();
                });
            } else {
                btn->set_callback([this, skin_id = skin.id, cost = skin.cost]() {
                    if (core::LevelProgress::instance().spend_coins(cost)) {
                        core::LevelProgress::instance().unlock_skin(skin_id);
                        core::LevelProgress::instance().select_skin(skin_id);
                        create_ui();
                    } else {
                        std::cout << "Not enough coins!" << std::endl;
                    }
                });
            }
            
            m_buttons.push_back(std::move(btn));
        }
        
        // Back Button
        auto back_btn = std::make_unique<ui::UIButton>(
            sf::Vector2f(50.0f, 650.0f),
            sf::Vector2f(150.0f, 50.0f),
            "BACK",
            "button_cosmic",
            "click_sound",
            rm.get_font("cosmic_font")
        );
        back_btn->set_callback([this]() {
            m_state_manager.change_state(std::make_unique<MainMenuState>(m_state_manager));
        });
        m_buttons.push_back(std::move(back_btn));
    }

    void SkinSelectionState::handle_input() {
        auto mouse_pos_i = sf::Mouse::getPosition(m_state_manager.get_window().get_sf_window());
        sf::Vector2f mouse_pos(static_cast<float>(mouse_pos_i.x), static_cast<float>(mouse_pos_i.y));
        bool mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        for (auto& btn : m_buttons) {
            btn->update(mouse_pos, mouse_pressed);
        }
    }

    void SkinSelectionState::update(float dt) {
        (void)dt;
    }

    void SkinSelectionState::draw(core::GameWindow& window) {
        // Draw background
        if (m_background) {
            window.draw(*m_background);
        }
        
        // Draw title
        window.draw(m_title);
        
        // Draw decorative dividers
        if (m_divider_top) window.draw(*m_divider_top);
        if (m_divider_bottom) window.draw(*m_divider_bottom);
        
        // Draw wallet
        if (m_wallet_coin_sprite) window.draw(*m_wallet_coin_sprite);
        window.draw(m_wallet_text);
        
        // Draw skin circles (blue backgrounds)
        for (const auto& circle : m_skin_circles) {
            window.draw(circle);
        }
        
        // Draw helmet previews inside circles
        for (const auto& preview : m_skin_previews) {
            window.draw(preview);
        }
        
        // Draw checkmarks for selected skin
        for (size_t i = 0; i < m_selected_checks.size() && i < m_is_selected.size(); ++i) {
            if (m_is_selected[i]) {
                window.draw(m_selected_checks[i]);
            }
        }
        
        // Draw skin labels
        for (const auto& label : m_skin_labels) {
            window.draw(label);
        }
        
        // Draw buttons
        for (auto& btn : m_buttons) {
            btn->render(window);
        }
    }

} // namespace states
