#include "StateManager.hpp"
#include "../core/GameWindow.hpp"

namespace states {

    StateManager::StateManager(core::GameWindow& window)
        : m_window(window) {}

    void StateManager::push_state(std::unique_ptr<State> state) {
        m_is_adding = true;
        m_temp_state = std::move(state);
    }

    void StateManager::pop_state() {
        m_is_removing = true;
    }

    void StateManager::change_state(std::unique_ptr<State> state) {
        m_is_replacing = true;
        m_temp_state = std::move(state);
    }

    void StateManager::process_state_changes() {
        if (m_is_removing && !m_states.empty()) {
            m_states.pop();
            if (!m_states.empty()) {
                m_states.top()->resume();
            }
            m_is_removing = false;
        }

        if (m_is_adding) {
            if (!m_states.empty()) {
                m_states.top()->pause();
            }
            m_states.push(std::move(m_temp_state));
            m_states.top()->init();
            m_is_adding = false;
        }

        if (m_is_replacing && !m_states.empty()) {
            m_states.pop();
            m_states.push(std::move(m_temp_state));
            m_states.top()->init();
            m_is_replacing = false;
        }
    }

    State* StateManager::get_current_state() {
        if (m_states.empty()) return nullptr;
        return m_states.top().get();
    }

    bool StateManager::is_empty() const {
        return m_states.empty();
    }

    void StateManager::handle_input() {
        if (!m_states.empty()) {
            m_states.top()->handle_input();
        }
    }

    void StateManager::update(float dt) {
        if (!m_states.empty()) {
            m_states.top()->update(dt);
        }
    }

    void StateManager::draw() {
        if (!m_states.empty()) {
            m_states.top()->draw(m_window);
        }
    }

} // namespace states
