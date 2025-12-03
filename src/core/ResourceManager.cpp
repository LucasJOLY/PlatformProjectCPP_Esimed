#include "ResourceManager.hpp"
#include <iostream>

namespace core {

    ResourceManager& ResourceManager::instance() {
        static ResourceManager s_instance;
        return s_instance;
    }

    sf::Texture& ResourceManager::load_texture(const std::string& name, const std::filesystem::path& path) {
        if (m_textures.contains(name)) {
            return m_textures.at(name);
        }

        sf::Texture texture;
        if (!texture.loadFromFile(path.string())) {
            std::cerr << "[WARNING] Failed to load texture: " << path << ". Using fallback." << std::endl;
            texture = create_fallback_texture();
        }

        auto [it, inserted] = m_textures.emplace(name, std::move(texture));
        return it->second;
    }

    sf::Texture& ResourceManager::get_texture(const std::string& name) {
        if (m_textures.find(name) == m_textures.end()) {
            std::cerr << "[ERROR] Texture not found: " << name << ". Returning fallback." << std::endl;
            static sf::Texture fallback = create_fallback_texture();
            return fallback;
        }
        return m_textures[name];
    }

    bool ResourceManager::has_texture(const std::string& name) const {
        return m_textures.find(name) != m_textures.end();
    }

    bool ResourceManager::has_font(const std::string& name) const {
        return m_fonts.contains(name);
    }

    bool ResourceManager::has_sound_buffer(const std::string& name) const {
        return m_sound_buffers.contains(name);
    }

    void ResourceManager::play_sound(const std::string& name) {
        if (!has_sound_buffer(name)) {
             std::cerr << "[WARNING] Cannot play sound '" << name << "': Buffer not found." << std::endl;
             return;
        }
        
        // Create a new sound instance
        m_active_sounds.emplace_back(get_sound_buffer(name));
        m_active_sounds.back().play();
        
        // Clean up finished sounds
        clean_sounds();
    }

    void ResourceManager::clean_sounds() {
        m_active_sounds.remove_if([](const sf::Sound& sound) {
            return sound.getStatus() == sf::Sound::Status::Stopped;
        });
    }

    sf::Font& ResourceManager::load_font(const std::string& name, const std::filesystem::path& path) {
        if (m_fonts.contains(name)) {
            return m_fonts.at(name);
        }

        sf::Font font;
        if (!font.openFromFile(path.string())) {
             std::cerr << "[ERROR] Failed to load font: " << path << ". Trying system fallback." << std::endl;
             if (!font.openFromFile("/System/Library/Fonts/AppleSDGothicNeo.ttc")) {
                 std::cerr << "[CRITICAL] Failed to load system fallback font!" << std::endl;
                 // If even fallback fails, we are in trouble. But let's hope it doesn't.
             }
        }

        auto [it, inserted] = m_fonts.emplace(name, std::move(font));
        return it->second;
    }

    sf::Font& ResourceManager::get_font(const std::string& name) {
        if (m_fonts.contains(name)) {
            return m_fonts.at(name);
        }
        
        // If not found, try to load fallback system font if not already loaded
        if (!m_fonts.contains("system_fallback")) {
             std::cerr << "[WARNING] Font '" << name << "' not found. Loading system fallback." << std::endl;
             load_font("system_fallback", "/System/Library/Fonts/AppleSDGothicNeo.ttc");
        }
        
        if (m_fonts.contains("system_fallback")) {
            return m_fonts.at("system_fallback");
        }

        // If even fallback is missing (shouldn't happen on Mac), return a dummy reference? 
        // This is dangerous but we need to return something. 
        // Ideally we should have a default font embedded or guaranteed.
        // For now, let's just return the first available font or crash safely.
        static sf::Font empty_font;
        return empty_font;
    }

    sf::SoundBuffer& ResourceManager::load_sound_buffer(const std::string& name, const std::filesystem::path& path) {
        if (m_sound_buffers.contains(name)) {
            return m_sound_buffers.at(name);
        }

        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(path.string())) {
            std::cerr << "[ERROR] Failed to load sound buffer: " << path << std::endl;
            // We could return a dummy buffer or handle this better
        }

        auto [it, inserted] = m_sound_buffers.emplace(name, std::move(buffer));
        return it->second;
    }

    sf::SoundBuffer& ResourceManager::get_sound_buffer(const std::string& name) {
        if (m_sound_buffers.contains(name)) {
            return m_sound_buffers.at(name);
        }
        std::cerr << "[ERROR] Sound buffer not found: " << name << std::endl;
        static sf::SoundBuffer empty_buffer;
        return empty_buffer;
    }

    sf::Texture ResourceManager::create_fallback_texture() {
        sf::Image image;
        image.resize({32, 32}, sf::Color::Magenta); 
        
        for(unsigned int x = 0; x < 32; ++x) {
            for(unsigned int y = 0; y < 32; ++y) {
                if ((x / 8 + y / 8) % 2 == 0) {
                    image.setPixel({x, y}, sf::Color::Black);
                }
            }
        }

        sf::Texture texture;
        if (!texture.loadFromImage(image)) {
            std::cerr << "[ERROR] Failed to create fallback texture from image." << std::endl;
        }
        return texture;
    }

} // namespace core
