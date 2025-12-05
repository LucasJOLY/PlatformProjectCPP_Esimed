#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "../core/GameWindow.hpp"
#include "../entities/Player.hpp"
#include "../entities/Enemy.hpp"
#include "../entities/FlyingEnemy.hpp"
#include "../entities/Coin.hpp"
#include "TileMap.hpp"

namespace world {

    class World {
    public:
        explicit World(int level_id);
        explicit World(const std::string& custom_level_data);  // For custom levels
        ~World() = default;

        void update(float dt);
        void render(core::GameWindow& window);

    private:
        int m_level_id;
        std::unique_ptr<entities::Player> m_player;
        std::vector<std::unique_ptr<entities::Enemy>> m_enemies;
        std::vector<std::unique_ptr<entities::FlyingEnemy>> m_flying_enemies;
        std::vector<std::unique_ptr<entities::Coin>> m_coins;
        TileMap m_tilemap;
        sf::Vector2f m_checkpoint_position;
        bool m_level_complete;
        bool m_game_over;
        int m_coins_collected;
        int m_total_coins;
        
        // Camera
        sf::View m_camera;
        
        void handle_collisions();
        void handle_enemy_collisions();
        void check_player_enemy_collision();
        void check_flag_collision();
        void check_checkpoint_collision();
        void check_coin_collision();
        void update_camera();
        std::string get_level_data(int level_id);
        
    public:
        bool is_level_complete() const { return m_level_complete; }
        bool is_game_over() const { return m_game_over; }
        int get_player_lives() const { return m_player ? m_player->get_lives() : 0; }
        int get_coins_collected() const { return m_coins_collected; }
        int get_total_coins() const { return m_total_coins; }
        const sf::View& get_camera() const { return m_camera; }
    };

} // namespace world
