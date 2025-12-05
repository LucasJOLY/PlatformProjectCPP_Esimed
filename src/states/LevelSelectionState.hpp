#pragma once

#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "../ui/UIButton.hpp"
#include <vector>
#include <memory>

namespace states {

    class LevelSelectionState : public State {
    public:
        explicit LevelSelectionState(StateManager& state_manager);
        ~LevelSelectionState() override = default;

        void init() override;
        void handle_input() override;
        void update(float dt) override;
        void draw(core::GameWindow& window) override;

    private:
        void create_standard_level_buttons();
        void create_custom_level_buttons();
        void switch_view();
        
        StateManager& m_state_manager;
        sf::Sprite m_background;
        sf::Text m_title;
        
        // UI Buttons
        std::vector<std::unique_ptr<ui::UIButton>> m_level_buttons;  // Current view buttons (standard or custom)
        std::unique_ptr<ui::UIButton> m_back_button;
        std::unique_ptr<ui::UIButton> m_toggle_button;  // Button to switch between views
        
        // For locked levels in standard view
        std::vector<sf::Sprite> m_lock_sprites;
        std::vector<std::vector<sf::Sprite>> m_star_sprites;
        
        // State
        bool m_showing_custom = false;  // false = standard levels, true = custom levels
    };

} // namespace states
