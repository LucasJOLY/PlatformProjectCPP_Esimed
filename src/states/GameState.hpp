#pragma once

#include "State.hpp"
#include "StateManager.hpp"
#include "../core/GameWindow.hpp"
#include "../world/World.hpp"
#include "../ui/UIButton.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

namespace states {

    class GameState : public State {
    public:
        GameState(StateManager& state_manager, int level_id);
        GameState(StateManager& state_manager, const std::string& custom_data, bool is_test_mode);  // For custom levels
        ~GameState() override = default;

        void init() override;
        void handle_input() override;
        void update(float dt) override;
        void draw(core::GameWindow& window) override;

    private:
        void create_menu_button(bool is_victory);
        
        StateManager& m_state_manager;
        int m_level_id;
        std::string m_custom_data;
        bool m_is_test_mode = false;
        std::unique_ptr<world::World> m_world;
        
        // HUD
        std::optional<sf::Text> m_lives_text;
        std::optional<sf::Text> m_status_text;
        std::vector<sf::Sprite> m_life_sprites;
        std::optional<sf::Sprite> m_panel_sprite;
        std::optional<sf::Sprite> m_divider_sprite;
        
        // Menu button
        std::unique_ptr<ui::UIButton> m_action_button;
        sf::Vector2f m_mouse_pos;
        bool m_mouse_pressed = false;
        bool m_menu_shown = false;
        
        // Audio
        sf::SoundBuffer m_jump_buffer;
        sf::SoundBuffer m_damage_buffer;
        sf::SoundBuffer m_victory_buffer;
        std::optional<sf::Sound> m_jump_sound;
        std::optional<sf::Sound> m_damage_sound;
        std::optional<sf::Sound> m_victory_sound;
    };

} // namespace states
