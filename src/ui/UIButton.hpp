#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <functional>
#include <string>

namespace core {
    class GameWindow;
}

namespace ui {

    class UIButton {
    public:
        // Constructor: texture_name is the background tile, sound_name is the click sound
        UIButton(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text, const std::string& texture_name, const std::string& sound_name, const sf::Font& font);
        ~UIButton();

        void update(const sf::Vector2f& mouse_pos, bool mouse_pressed);
        void render(core::GameWindow& window);

        void set_callback(std::function<void()> callback);
        void set_icon(const sf::Texture& texture);
        void set_label(const std::string& label);
        
        sf::Vector2f get_position() const { return {m_bounds.position.x, m_bounds.position.y}; }
        sf::Vector2f get_size() const { return {m_bounds.size.x, m_bounds.size.y}; }

    private:
        std::optional<sf::Sprite> m_sprite;
        std::optional<sf::Sprite> m_icon;
        std::optional<sf::Text> m_text;
        std::string m_sound_name;
        
        sf::FloatRect m_bounds;
        
        bool m_is_hovered = false;
        bool m_is_pressed = false;
        
        std::function<void()> m_callback;

        // Visual tweaks
        sf::Color m_normal_color = sf::Color::White;
        sf::Color m_hover_color = {220, 220, 220};
        sf::Color m_pressed_color = {180, 180, 180};
    };

} // namespace ui
