#pragma once

#include "State.hpp"
#include <stack>
#include <memory>
#include <functional>

namespace core {
    class GameWindow;
}

namespace states {

    class StateManager {
    public:
        explicit StateManager(core::GameWindow& window);

        void push_state(std::unique_ptr<State> state);
        void pop_state();
        void change_state(std::unique_ptr<State> state);

        void process_state_changes();
        
        [[nodiscard]] State* get_current_state();
        [[nodiscard]] bool is_empty() const;

        // Main loop delegates
        void handle_input();
        void update(float dt);
        void draw();

        [[nodiscard]] core::GameWindow& get_window() { return m_window; }

    private:
        std::stack<std::unique_ptr<State>> m_states;
        core::GameWindow& m_window;

        bool m_is_removing = false;
        bool m_is_adding = false;
        bool m_is_replacing = false;
        std::unique_ptr<State> m_temp_state;
    };

} // namespace states
