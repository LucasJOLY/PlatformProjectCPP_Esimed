#include "GameWindow.hpp"

namespace core {

    GameWindow::GameWindow(unsigned int width, unsigned int height, const std::string& title)
        : m_window(sf::VideoMode({width, height}), title) {
        m_window.setFramerateLimit(60);
    }

    bool GameWindow::is_open() const {
        return m_window.isOpen();
    }

    void GameWindow::poll_events() {
        while (const std::optional event = m_window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                m_window.close();
            }
        }
    }

    void GameWindow::clear(sf::Color color) {
        m_window.clear(color);
    }

    void GameWindow::display() {
        m_window.display();
    }

    void GameWindow::draw(const sf::Drawable& drawable) {
        m_window.draw(drawable);
    }

    sf::RenderWindow& GameWindow::get_sf_window() {
        return m_window;
    }

} // namespace core
