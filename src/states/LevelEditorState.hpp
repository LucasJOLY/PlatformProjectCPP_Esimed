#pragma once

#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateManager.hpp"
#include "../ui/UIButton.hpp"
#include <vector>
#include <memory>
#include <string>

namespace states {

    enum class EditorTool {
        Block,
        Erase,
        Player,
        Flag,
        Enemy,
        FlyingEnemy,
        Checkpoint
    };

    enum class MapSize {
        Small,   // 20x10
        Medium,  // 40x10
        Large    // 60x10
    };

    class LevelEditorState : public State {
    public:
        // level_id: -1 for new level, positive for editing existing
        LevelEditorState(StateManager& state_manager, int level_id);
        ~LevelEditorState() override = default;

        void init() override;
        void handle_input() override;
        void update(float dt) override;
        void draw(core::GameWindow& window) override;

    private:
        void init_grid();
        void reset_grid();
        void load_level(int level_id);
        void place_tile(int col, int row);
        void erase_tile(int col, int row);
        bool validate_level();
        std::string generate_level_data();
        void save_level();
        void change_map_size(MapSize size);
        void create_toolbar_buttons();

        StateManager& m_state_manager;
        int m_level_id;
        
        // Background
        sf::Sprite m_background;
        sf::Text m_title;
        
        // Grid - now dynamic
        int m_grid_cols = 40;
        int m_grid_rows = 10;
        static constexpr float TILE_SIZE = 28.0f;
        static constexpr float GRID_START_X = 40.0f;
        static constexpr float GRID_START_Y = 100.0f;
        
        std::vector<std::vector<char>> m_grid;
        
        // Current tool
        EditorTool m_current_tool = EditorTool::Block;
        MapSize m_current_map_size = MapSize::Medium;
        
        // Track placed entities (only one allowed)
        bool m_player_placed = false;
        bool m_flag_placed = false;
        int m_player_col = -1, m_player_row = -1;
        int m_flag_col = -1, m_flag_row = -1;
        
        // UI
        std::vector<std::unique_ptr<ui::UIButton>> m_toolbar_buttons;
        std::vector<std::unique_ptr<ui::UIButton>> m_action_buttons;
        std::vector<std::unique_ptr<ui::UIButton>> m_size_buttons;
        
        // Input state
        bool m_was_mouse_pressed = false;
    };

} // namespace states
