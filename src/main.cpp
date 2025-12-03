#include "core/GameWindow.hpp"
#include "core/ResourceManager.hpp"
#include "states/StateManager.hpp"
#include "states/MainMenuState.hpp"
#include <iostream>

int main() {
    std::cout << "Starting PlatformProjectCPP_Esimed..." << std::endl;

    // Initialize Window
    core::GameWindow window(1280, 720, "Terraquest Platformer");

    // Initialize State Manager
    states::StateManager state_manager(window);
    state_manager.push_state(std::make_unique<states::MainMenuState>(state_manager));

    // Clock for dt
    sf::Clock clock;

    // Game Loop
    while (window.is_open()) {
        // Calculate Delta Time
        sf::Time dt_time = clock.restart();
        float dt = dt_time.asSeconds();

        window.poll_events();
        
        // State Loop
        state_manager.handle_input();
        state_manager.update(dt);
        state_manager.process_state_changes();

        window.clear();
        state_manager.draw();
        window.display();
    }

    return 0;
}
