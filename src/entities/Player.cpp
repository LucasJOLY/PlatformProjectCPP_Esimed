#include "Player.hpp"
#include "../core/LevelProgress.hpp"
#include "../core/SkinManager.hpp"
#include <iostream>
#include <cmath>
#include <filesystem>
#include <string>
#include <SFML/Audio.hpp>

namespace entities {

    Player::Player(const sf::Vector2f& position)
        : Entity(position, sf::Vector2f(32.0f, 48.0f)),
          m_on_ground(false),
          m_lives(3),
          m_state(AnimationState::Idle),
          m_animation_timer(0.0f),
          m_facing_right(true),
          m_walk_frame(0) {
        
        // Get selected skin
        std::string skin_id = core::LevelProgress::instance().get_selected_skin();
        const auto& skin_info = core::SkinManager::instance().get_skin(skin_id);
        m_skin_prefix = skin_info.texture_prefix;
        
        // Load all animation textures with unique keys
        auto& rm = core::ResourceManager::instance();
        std::string idle_key = "player_" + m_skin_prefix + "_idle";
        std::string jump_key = "player_" + m_skin_prefix + "_jump";
        std::string walk_a_key = "player_" + m_skin_prefix + "_walk_a";
        std::string walk_b_key = "player_" + m_skin_prefix + "_walk_b";

        rm.load_texture(idle_key, "assets/Pack_to_pick/Game/Sprites/Characters/Default/" + m_skin_prefix + "_idle.png");
        rm.load_texture(jump_key, "assets/Pack_to_pick/Game/Sprites/Characters/Default/" + m_skin_prefix + "_jump.png");
        rm.load_texture(walk_a_key, "assets/Pack_to_pick/Game/Sprites/Characters/Default/" + m_skin_prefix + "_walk_a.png");
        rm.load_texture(walk_b_key, "assets/Pack_to_pick/Game/Sprites/Characters/Default/" + m_skin_prefix + "_walk_b.png");

        // Initialize sprite
        m_sprite.emplace(rm.get_texture(idle_key));

        // Scale sprite to match hitbox size
        const auto& tex = rm.get_texture(idle_key);
        sf::Vector2u tex_size = tex.getSize();
        m_sprite->setScale(sf::Vector2f(m_size.x / tex_size.x, m_size.y / tex_size.y));
        
        // Load and init sounds
        rm.load_sound_buffer("player_jump", "assets/Pack_to_pick/Game/Sounds/sfx_jump.ogg");
        // Assuming damage.ogg is in assets/sounds/ as requested, if not found, ResourceManager will handle or we should check
        if (std::filesystem::exists("assets/sounds/damage.ogg")) {
             rm.load_sound_buffer("player_damage", "assets/sounds/damage.ogg");
        } else {
             // Fallback to hurt sound if damage.ogg missing
             rm.load_sound_buffer("player_damage", "assets/Pack_to_pick/Game/Sounds/sfx_hurt.ogg");
        }
        
        m_jump_sound.emplace(rm.get_sound_buffer("player_jump"));
        m_damage_sound.emplace(rm.get_sound_buffer("player_damage"));
    }

    void Player::update(float dt) {
        handle_input();
        apply_gravity(dt);
        
        // Update position based on velocity
        m_position += m_velocity * dt;
        
        update_animation(dt);
        
        // Update sprite position
        if (m_sprite) {
            m_sprite->setPosition(sf::Vector2f(m_position.x + m_size.x / 2.0f, m_position.y)); // Center X, Top Y (origin adjusted in update_animation)
        }
    }

    void Player::update_animation(float dt) {
        if (!m_sprite) return;
        auto& rm = core::ResourceManager::instance();
        
        // Determine state
        if (!m_on_ground) {
            m_state = AnimationState::Jumping;
        } else if (std::abs(m_velocity.x) > 10.0f) {
            m_state = AnimationState::Walking;
        } else {
            m_state = AnimationState::Idle;
        }
        
        // Determine facing direction
        if (m_velocity.x > 0) m_facing_right = true;
        else if (m_velocity.x < 0) m_facing_right = false;
        
        // Update texture based on state
        std::string idle_key = "player_" + m_skin_prefix + "_idle";
        std::string jump_key = "player_" + m_skin_prefix + "_jump";
        std::string walk_a_key = "player_" + m_skin_prefix + "_walk_a";
        std::string walk_b_key = "player_" + m_skin_prefix + "_walk_b";

        switch (m_state) {
            case AnimationState::Idle:
                m_sprite->setTexture(rm.get_texture(idle_key));
                break;
                
            case AnimationState::Jumping:
                m_sprite->setTexture(rm.get_texture(jump_key));
                break;
                
            case AnimationState::Walking:
                m_animation_timer += dt;
                if (m_animation_timer >= ANIMATION_FRAME_TIME) {
                    m_animation_timer = 0.0f;
                    m_walk_frame = 1 - m_walk_frame; // Toggle between 0 and 1
                }
                
                if (m_walk_frame == 0) {
                    m_sprite->setTexture(rm.get_texture(walk_a_key));
                } else {
                    m_sprite->setTexture(rm.get_texture(walk_b_key));
                }
                break;
        }
        
        // Handle flipping
        // Reset scale to positive
        sf::Vector2f scale = m_sprite->getScale();
        scale.x = std::abs(scale.x);
        
        if (!m_facing_right) {
            scale.x = -scale.x;
            m_sprite->setOrigin(sf::Vector2f(m_sprite->getTexture().getSize().x / 2.0f, 0.0f));
            m_sprite->setPosition(sf::Vector2f(m_position.x + m_size.x / 2.0f, m_position.y));
        } else {
             m_sprite->setOrigin(sf::Vector2f(m_sprite->getTexture().getSize().x / 2.0f, 0.0f));
             m_sprite->setPosition(sf::Vector2f(m_position.x + m_size.x / 2.0f, m_position.y));
        }
        m_sprite->setScale(scale);
    }

    void Player::render(core::GameWindow& window) {
        if (m_sprite) {
            window.draw(*m_sprite);
        }
    }

    void Player::handle_input() {
        // Horizontal movement
        m_velocity.x = 0.0f;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || 
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
            m_velocity.x = -MOVE_SPEED;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || 
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            m_velocity.x = MOVE_SPEED;
        }
        
        // Jump
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || 
             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) && m_on_ground) {
            jump();
        }
    }

    void Player::apply_gravity(float dt) {
        if (!m_on_ground) {
            m_velocity.y += GRAVITY * dt;
            
            // Cap fall speed
            if (m_velocity.y > MAX_FALL_SPEED) {
                m_velocity.y = MAX_FALL_SPEED;
            }
        }
    }

    void Player::jump() {
        m_velocity.y = JUMP_VELOCITY;
        m_on_ground = false;
        if (m_jump_sound) m_jump_sound->play();
    }

    void Player::take_damage() {
        if (m_lives > 0) {
            m_lives--;
            if (m_damage_sound) m_damage_sound->play();
            std::cout << "Player took damage! Lives remaining: " << m_lives << std::endl;
        }
    }

    void Player::reset_to_checkpoint(const sf::Vector2f& checkpoint_pos) {
        m_position = checkpoint_pos;
        m_velocity = sf::Vector2f(0.0f, 0.0f);
        m_on_ground = false;
    }

} // namespace entities
