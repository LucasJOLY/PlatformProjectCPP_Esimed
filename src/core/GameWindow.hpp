#pragma once

#include <SFML/Graphics.hpp>
#include <string>

namespace core {

    class GameWindow {
    public:
        GameWindow(unsigned int width, unsigned int height, const std::string& title);

        [[nodiscard]] bool is_open() const;
        void poll_events();
        void clear(sf::Color color = sf::Color::Black);
        void display();
        void draw(const sf::Drawable& drawable);
        
        [[nodiscard]] sf::RenderWindow& get_sf_window();

    private:
        sf::RenderWindow m_window;
    };

} // namespace core
