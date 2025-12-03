#pragma once

#include "State.hpp"
#include "../ui/UIButton.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

namespace states {

    class StateManager;

    class MainMenuState : public State {
    public:
        explicit MainMenuState(StateManager& state_manager);

        void init() override;
        void handle_input() override;
        void update(float dt) override;
        void draw(core::GameWindow& window) override;

    private:
        StateManager& m_state_manager;
        
        sf::Sprite m_background;
        sf::Text m_title;
        std::vector<std::unique_ptr<ui::UIButton>> m_buttons;
    };

} // namespace states
