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
        StateManager& m_state_manager;
        sf::Sprite m_background;
        sf::Text m_title;
        std::vector<std::unique_ptr<ui::UIButton>> m_buttons;
        std::vector<sf::Sprite> m_lock_sprites;  // For locked levels
        std::vector<std::vector<sf::Sprite>> m_star_sprites;  // Stars for each level
    };

} // namespace states
