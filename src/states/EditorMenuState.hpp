#pragma once

#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "../ui/UIButton.hpp"
#include <vector>
#include <memory>

namespace states {

    class EditorMenuState : public State {
    public:
        explicit EditorMenuState(StateManager& state_manager);
        ~EditorMenuState() override = default;

        void init() override;
        void handle_input() override;
        void update(float dt) override;
        void draw(core::GameWindow& window) override;

    private:
        void refresh_level_list();
        
        StateManager& m_state_manager;
        sf::Sprite m_background;
        sf::Text m_title;
        
        std::vector<std::unique_ptr<ui::UIButton>> m_buttons;
        std::vector<std::unique_ptr<ui::UIButton>> m_level_buttons;  // Dynamic level list
        
        // Scrolling for level list
        float m_scroll_offset = 0.0f;
        float m_max_scroll = 0.0f;
    };

} // namespace states
