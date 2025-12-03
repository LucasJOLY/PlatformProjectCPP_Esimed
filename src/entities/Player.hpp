#pragma once

#include "Entity.hpp"
#include "../core/ResourceManager.hpp"

namespace entities {

    class Player : public Entity {
    public:
        Player(const sf::Vector2f& position);
        ~Player() override = default;

        void update(float dt) override;
        void render(core::GameWindow& window) override;

        void handle_input();
        void apply_gravity(float dt);
        void jump();
        
        bool is_on_ground() const { return m_on_ground; }
        void set_on_ground(bool on_ground) { m_on_ground = on_ground; }
        
        int get_lives() const { return m_lives; }
        void take_damage();
        void reset_to_checkpoint(const sf::Vector2f& checkpoint_pos);

        // Animation
        enum class AnimationState {
            Idle,
            Walking,
            Jumping
        };

#include <optional>

    private:
        std::optional<sf::Sprite> m_sprite;
        bool m_on_ground;
        int m_lives;
        
        // Animation members
        AnimationState m_state;
        float m_animation_timer;
        bool m_facing_right;
        int m_walk_frame; // 0 or 1 for walk_a / walk_b
        std::string m_skin_prefix;
        
        std::optional<sf::Sound> m_jump_sound;
        std::optional<sf::Sound> m_damage_sound;
        
        void update_animation(float dt);
        
        // Physics constants
        static constexpr float MOVE_SPEED = 200.0f;
        static constexpr float JUMP_VELOCITY = -500.0f;
        static constexpr float GRAVITY = 1200.0f;
        static constexpr float MAX_FALL_SPEED = 600.0f;
        static constexpr float ANIMATION_FRAME_TIME = 0.15f; // Time per frame
    };

} // namespace entities
