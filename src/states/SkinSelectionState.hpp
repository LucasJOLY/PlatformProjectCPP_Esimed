#pragma once

#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "../ui/UIButton.hpp"
#include <vector>
#include <memory>
#include <optional>

namespace states {

    class SkinSelectionState : public State {
    public:
        explicit SkinSelectionState(StateManager& state_manager);
        ~SkinSelectionState() override = default;

        void init() override;
        void handle_input() override;
        void update(float dt) override;
        void draw(core::GameWindow& window) override;

    private:
        void create_ui();

        StateManager& m_state_manager;
        std::optional<sf::Sprite> m_background;
        sf::Text m_title;
        sf::Text m_wallet_text;
        std::optional<sf::Sprite> m_wallet_coin_sprite;
        std::optional<sf::Sprite> m_divider_top;
        std::optional<sf::Sprite> m_divider_bottom;
        std::vector<std::unique_ptr<ui::UIButton>> m_buttons;
        std::vector<sf::Text> m_skin_labels;
        std::vector<sf::Sprite> m_skin_previews;
        std::vector<sf::Sprite> m_skin_circles;      // Blue circle backgrounds
        std::vector<sf::Sprite> m_selected_checks;   // Checkmarks for selected skin
        std::vector<bool> m_is_selected;             // Track which skin is selected
    };

} // namespace states
