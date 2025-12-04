#include "World.hpp"
#include <iostream>
#include <sstream>

namespace world {

    World::World(int level_id) : m_level_id(level_id), m_checkpoint_position(100.0f, 500.0f), 
                                  m_level_complete(false), m_game_over(false), 
                                  m_coins_collected(0), m_total_coins(0) {
        std::cout << "World initialized for Level " << m_level_id << std::endl;
        
        // Initialize camera
        m_camera.setSize(sf::Vector2f(800.0f, 600.0f));
        m_camera.setCenter(sf::Vector2f(400.0f, 300.0f));
        
        // Load level data
        std::string level_data = get_level_data(level_id);
        m_tilemap.load_from_string(level_data, level_id);
        
        // Create player at spawn position
        m_player = std::make_unique<entities::Player>(m_tilemap.get_spawn_position());
        m_checkpoint_position = m_tilemap.get_spawn_position();
        
        // Spawn enemies and coins from level data
        std::istringstream stream(level_data);
        std::string line;
        int row = 0;
        while (std::getline(stream, line)) {
            for (size_t col = 0; col < line.length(); ++col) {
                if (line[col] == 'E') {
                    sf::Vector2f enemy_pos(col * 32.0f, row * 32.0f);
                    m_enemies.push_back(std::make_unique<entities::Enemy>(enemy_pos));
                } else if (line[col] == 'V') {
                    sf::Vector2f fly_pos(col * 32.0f, row * 32.0f);
                    m_flying_enemies.push_back(std::make_unique<entities::FlyingEnemy>(fly_pos));
                } else if (line[col] == 'O') {
                    sf::Vector2f coin_pos(col * 32.0f, row * 32.0f);
                    m_coins.push_back(std::make_unique<entities::Coin>(coin_pos));
                    m_total_coins++;
                }
            }
            row++;
        }
        
        std::cout << "Spawned " << m_enemies.size() << " enemies" << std::endl;
        std::cout << "Spawned " << m_flying_enemies.size() << " flying enemies" << std::endl;
        std::cout << "Spawned " << m_total_coins << " coins" << std::endl;
    }

    void World::update(float dt) {
        // Don't update if game is over or level complete
        if (m_game_over || m_level_complete) return;
        
        if (m_player) {
            m_player->update(dt);
            handle_collisions();
            update_camera();
        }
        
        // Update enemies
        for (auto& enemy : m_enemies) {
            enemy->update(dt);
        }
        
        // Update flying enemies
        for (auto& fly : m_flying_enemies) {
            fly->update(dt);
        }
        
        // Update coins (animation?)
        // for (auto& coin : m_coins) coin->update(dt);
        
        handle_enemy_collisions();
        check_player_enemy_collision();
        check_flag_collision();
        check_checkpoint_collision();
        check_coin_collision();
    }
    
    void World::update_camera() {
        if (!m_player) return;
        
        sf::Vector2f player_pos = m_player->get_position();
        sf::Vector2f camera_center = m_camera.getCenter();
        
        // Follow player horizontally with smooth movement
        float target_x = player_pos.x + 16.0f; // Center on player (half of 32px)
        float camera_x = camera_center.x + (target_x - camera_center.x) * 0.1f;
        
        // Get level bounds
        float level_width = m_tilemap.get_width() * 32.0f;
        float camera_half_width = 400.0f; // Half of 800px screen width
        
        // Constrain camera to level bounds
        if (camera_x - camera_half_width < 0.0f) {
            camera_x = camera_half_width;
        } else if (camera_x + camera_half_width > level_width) {
            camera_x = level_width - camera_half_width;
        }
        
        // Update camera position (keep Y centered)
        m_camera.setCenter(sf::Vector2f(camera_x, 300.0f));
    }

    void World::render(core::GameWindow& window) {
        m_tilemap.render(window, m_camera);
        
        // Render coins
        for (const auto& coin : m_coins) {
            coin->render(window);
        }
        
        // Render enemies
        for (const auto& enemy : m_enemies) {
            enemy->render(window);
        }
        
        // Render flying enemies
        for (const auto& fly : m_flying_enemies) {
            fly->render(window);
        }
        
        if (m_player) {
            m_player->render(window);
        }
    }

    void World::handle_collisions() {
        // Simple AABB collision with tiles
        auto solid_tiles = m_tilemap.get_solid_tiles();
        sf::FloatRect player_bounds = m_player->get_bounds();
        
        bool on_ground = false;
        
        for (const auto& tile : solid_tiles) {
            sf::FloatRect tile_bounds = tile.get_bounds();
            
            if (player_bounds.findIntersection(tile_bounds)) {
                // Calculate overlap
                float overlap_left = (player_bounds.position.x + player_bounds.size.x) - tile_bounds.position.x;
                float overlap_right = (tile_bounds.position.x + tile_bounds.size.x) - player_bounds.position.x;
                float overlap_top = (player_bounds.position.y + player_bounds.size.y) - tile_bounds.position.y;
                float overlap_bottom = (tile_bounds.position.y + tile_bounds.size.y) - player_bounds.position.y;
                
                // Find minimum overlap
                float min_overlap = std::min({overlap_left, overlap_right, overlap_top, overlap_bottom});
                
                sf::Vector2f pos = m_player->get_position();
                sf::Vector2f vel = m_player->get_velocity();
                
                if (min_overlap == overlap_top && vel.y > 0) {
                    // Collision from top (player landing on tile)
                    pos.y = tile_bounds.position.y - player_bounds.size.y;
                    vel.y = 0;
                    on_ground = true;
                } else if (min_overlap == overlap_bottom && vel.y < 0) {
                    // Collision from bottom (player hitting ceiling)
                    pos.y = tile_bounds.position.y + tile_bounds.size.y;
                    vel.y = 0;
                } else if (min_overlap == overlap_left) {
                    // Collision from left
                    pos.x = tile_bounds.position.x - player_bounds.size.x;
                    vel.x = 0;
                } else if (min_overlap == overlap_right) {
                    // Collision from right
                    pos.x = tile_bounds.position.x + tile_bounds.size.x;
                    vel.x = 0;
                }
                
                m_player->set_position(pos);
                m_player->set_velocity(vel);
            }
        }
        
        m_player->set_on_ground(on_ground);
    }

    void World::handle_enemy_collisions() {
        auto solid_tiles = m_tilemap.get_solid_tiles();
        std::vector<sf::FloatRect> tile_bounds;
        for (const auto& tile : solid_tiles) {
            tile_bounds.push_back(tile.get_bounds());
        }
        
        for (auto& enemy : m_enemies) {
            enemy->check_wall_collision(tile_bounds);
        }
    }

    void World::check_player_enemy_collision() {
        if (!m_player) return;
        
        sf::FloatRect player_bounds = m_player->get_bounds();
        
        for (const auto& enemy : m_enemies) {
            sf::FloatRect enemy_bounds = enemy->get_bounds();
            
            if (player_bounds.findIntersection(enemy_bounds)) {
                // Player hit enemy - take damage and respawn
                m_player->take_damage();
                
                if (m_player->get_lives() > 0) {
                    m_player->reset_to_checkpoint(m_checkpoint_position);
                    std::cout << "Player respawned at checkpoint!" << std::endl;
                } else {
                    m_game_over = true;
                    std::cout << "Game Over!" << std::endl;
                }
                return; // Exit function to avoid double damage/death in same frame
            }
        }
        
        // Check flying enemies
        for (const auto& fly : m_flying_enemies) {
            sf::FloatRect enemy_bounds = fly->get_bounds();
            
            if (player_bounds.findIntersection(enemy_bounds)) {
                // Player hit enemy - take damage and respawn
                m_player->take_damage();
                
                if (m_player->get_lives() > 0) {
                    m_player->reset_to_checkpoint(m_checkpoint_position);
                    std::cout << "Player respawned at checkpoint!" << std::endl;
                } else {
                    m_game_over = true;
                    std::cout << "Game Over!" << std::endl;
                }
                return;
            }
        }
    }

    void World::check_flag_collision() {
        if (!m_player) return;
        
        sf::Vector2f flag_pos = m_tilemap.get_flag_position();
        sf::FloatRect flag_bounds(flag_pos, sf::Vector2f(32.0f, 32.0f));
        sf::FloatRect player_bounds = m_player->get_bounds();
        
        if (player_bounds.findIntersection(flag_bounds)) {
            m_level_complete = true;
            std::cout << "Level Complete!" << std::endl;
        }
    }
    
    void World::check_checkpoint_collision() {
        if (!m_player) return;
        
        auto checkpoint_positions = m_tilemap.get_checkpoint_positions();
        sf::FloatRect player_bounds = m_player->get_bounds();
        
        for (const auto& checkpoint_pos : checkpoint_positions) {
            sf::FloatRect checkpoint_bounds(checkpoint_pos, sf::Vector2f(32.0f, 32.0f));
            
            if (player_bounds.findIntersection(checkpoint_bounds)) {
                // Update checkpoint position if it's different
                if (m_checkpoint_position != checkpoint_pos) {
                    m_checkpoint_position = checkpoint_pos;
                    m_tilemap.activate_checkpoint(checkpoint_pos);
                    std::cout << "Checkpoint activated!" << std::endl;
                }
            }
        }
    }
    
    void World::check_coin_collision() {
        if (!m_player) return;
        
        sf::FloatRect player_bounds = m_player->get_bounds();
        
        for (auto& coin : m_coins) {
            if (!coin->is_collected()) {
                sf::FloatRect coin_bounds = coin->get_bounds();
                
                if (player_bounds.findIntersection(coin_bounds)) {
                    coin->collect();
                    m_coins_collected++;
                    std::cout << "Coin collected! (" << m_coins_collected << "/" << m_total_coins << ")" << std::endl;
                }
            }
        }
    }

    std::string World::get_level_data(int level_id) {
        switch (level_id) {
            case 1: // Tutorial: Basic platforming (no enemies)
                return 
                    "#                                      #\n"
                    "#                                      #\n"
                    "#                                      #\n"
                    "#                                      #\n"
                    "#               O                      #\n"
                    "#  P      O         O                F #\n"
                    "##                              ########\n"
                    "##      ###   O   ###   O   ############\n"
                    "########################################\n"
                    "########################################\n";
                    
            case 2: // Introduction to enemies
                return 
                    "#                                      #\n"
                    "#                                      #\n"
                    "#               O                      #\n"
                    "#                                      #\n"
                    "#  P      O              O           F #\n"
                    "##           #E#              ##########\n"
                    "##      ###          ###   O   #########\n"
                    "########################################\n"
                    "########################################\n";
                    
            case 3: // Platforming challenge with flying enemies
                return 
                    "#                                                                                                  #\n"
                    "#                                                                                                  #\n"
                    "#                                                                                                  #\n"
                    "#                                                                                                  #\n"
                    "#                    O             O                          O                                    #\n"
                    "#  P      O      V          V               V         C               V                          F #\n"
                    "##           ######    ######       ######      ########      ######       ######       ###########\n"
                    "##                                                                                        #########\n"
                    "##    O   ###       ##       ##        ###      ##       ###       ##       ###      ###############\n"
                    "####################################################################################################\n"
                    "####################################################################################################\n";
                    
            case 4: // Enemies and flying enemies combined
                return 
                    "#                                                                                                  #\n"
                    "#                                                                                                  #\n"
                    "#                                                                                                  #\n"
                    "#                O             O                          O                        O               #\n"
                    "#  P      O                                                                                      F #\n"
                    "##         #E#        #E#             V        C        V           #E#            V    ###########\n"
                    "##         ###        ###        ######      #####     ######       ###        ####################\n"
                    "####################################################################################################\n"
                    "####################################################################################################\n";
                    
            case 5: // Final challenge with many enemies
                return 
                    "#                                                                                                                      #\n"
                    "#                                                                                                                      #\n"
                    "#                                                                                                                      #\n"
                    "#                         O             O                          O                          O                        #\n"
                    "#  P      O                                                                                                          F #\n"
                    "##                             V    #E#     C       V        #E#       V         #E#       V         ############\n"
                    "##         ##        ###       ##        ###   #####   ######     ###      ######     ###      ######     ############\n"
                    "##     #####    E    ###    E    ####     #####        ###        ####     #####     #####     ###  #########################\n"
                    "###########################################################################################################################\n"
                    "###########################################################################################################################\n";
                    
            default:
                return get_level_data(1);
        }
    }

} // namespace world
