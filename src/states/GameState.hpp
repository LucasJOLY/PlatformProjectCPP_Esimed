#pragma once

#include "State.hpp"
#include "StateManager.hpp"
#include "../core/GameWindow.hpp"
#include "../world/World.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

namespace states {

    class GameState : public State {
    public:
        GameState(StateManager& state_manager, int level_id);
        ~GameState() override = default;

        void init() override;
        void handle_input() override;
        void update(float dt) override;
        void draw(core::GameWindow& window) override;

    private:
        StateManager& m_state_manager;
        int m_level_id;
        std::unique_ptr<world::World> m_world;
        
        // HUD
        std::optional<sf::Text> m_lives_text;
        std::optional<sf::Text> m_status_text;
        std::vector<sf::Sprite> m_life_sprites;
        std::optional<sf::Sprite> m_panel_sprite;
        
        // Audio
        sf::SoundBuffer m_jump_buffer;
        sf::SoundBuffer m_damage_buffer;
        sf::SoundBuffer m_victory_buffer;
        std::optional<sf::Sound> m_jump_sound;
        std::optional<sf::Sound> m_damage_sound;
        std::optional<sf::Sound> m_victory_sound;
    };

} // namespace states
