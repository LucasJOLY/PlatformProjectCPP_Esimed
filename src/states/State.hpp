#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

namespace core {
    class GameWindow;
    class StateManager;
}

namespace states {

    class State {
    public:
        virtual ~State() = default;

        virtual void init() = 0;
        virtual void handle_input() = 0;
        virtual void update(float dt) = 0;
        virtual void draw(core::GameWindow& window) = 0;

        virtual void pause() {}
        virtual void resume() {}
    };

} // namespace states
