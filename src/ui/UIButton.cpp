#include "UIButton.hpp"
#include "../core/GameWindow.hpp"
#include "../core/ResourceManager.hpp"

namespace ui {

    UIButton::UIButton(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text, 
                       const std::string& texture_name, const std::string& sound_name, const sf::Font& font) {
        (void)sound_name; // Unused for now
        
        // Load button texture
        auto& texture = core::ResourceManager::instance().get_texture(texture_name);
        m_sprite = sf::Sprite(texture);
        m_sprite->setPosition(position);
        
        // Scale sprite to fit button size
        sf::Vector2u tex_size = texture.getSize();
        m_sprite->setScale(sf::Vector2f(size.x / tex_size.x, size.y / tex_size.y));
        
        // Setup text - SFML 3 constructor: Text(font, string, characterSize)
        m_text = sf::Text(font, text, 24);
        m_text->setFillColor(sf::Color::White);
        
        // Center text on button
        sf::FloatRect text_bounds = m_text->getLocalBounds();
        m_text->setOrigin(sf::Vector2f(text_bounds.position.x + text_bounds.size.x / 2.0f, text_bounds.position.y + text_bounds.size.y / 2.0f));
        m_text->setPosition(sf::Vector2f(position.x + size.x / 2.0f, position.y + size.y / 2.0f - 5.0f));
        
        // Store sound name
        m_sound_name = sound_name;

        m_bounds = m_sprite->getGlobalBounds(); // Initialize m_bounds after sprite setup
    }

    UIButton::~UIButton() {
        // No need to stop sound manually, ResourceManager handles it or it plays to completion
    }

    void UIButton::update(const sf::Vector2f& mouse_pos, bool mouse_pressed) {
        m_is_hovered = m_bounds.contains(mouse_pos);

        if (m_is_hovered) {
            if (mouse_pressed) {
                m_is_pressed = true;
                if (m_sprite) m_sprite->setColor(m_pressed_color);
            } else {
                if (m_is_pressed) {
                    // Click event on release
                    if (!m_sound_name.empty()) {
                        core::ResourceManager::instance().play_sound(m_sound_name);
                    }
                    
                    if (m_callback) {
                        m_callback(); 
                    }
                }
                m_is_pressed = false;
                if (m_sprite) m_sprite->setColor(m_hover_color);
            }
        } else {
            m_is_pressed = false;
            if (m_sprite) m_sprite->setColor(m_normal_color);
        }
    }

    void UIButton::render(core::GameWindow& window) {
        if (m_sprite) window.draw(*m_sprite);
        if (m_icon) window.draw(*m_icon);
        if (m_text) window.draw(*m_text);
    }

    void UIButton::set_callback(std::function<void()> callback) {
        m_callback = std::move(callback);
    }

    void UIButton::set_icon(const sf::Texture& texture) {
        m_icon.emplace(texture);
        // Scale icon to fit height (e.g., 60% of button height)
        float icon_size = m_bounds.size.y * 0.6f;
        sf::Vector2u tex_size = texture.getSize();
        m_icon->setScale({icon_size / tex_size.x, icon_size / tex_size.y});
        
        // Re-center text and icon
        if (m_text) {
            sf::FloatRect text_bounds = m_text->getLocalBounds();
            float total_width = icon_size + 10.0f + text_bounds.size.x; // 10px spacing
            
            float start_x = m_bounds.position.x + (m_bounds.size.x - total_width) / 2.0f;
            float center_y = m_bounds.position.y + m_bounds.size.y / 2.0f;
            
            m_icon->setPosition({start_x, center_y - icon_size / 2.0f});
            
            // Adjust text pos (origin was centered in constructor, reset it to left-center)
            m_text->setOrigin({text_bounds.position.x, text_bounds.position.y + text_bounds.size.y / 2.0f});
            m_text->setPosition({start_x + icon_size + 10.0f, center_y - 5.0f});
        }
    }

} // namespace ui
