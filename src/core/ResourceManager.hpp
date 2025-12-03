#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <filesystem>
#include <iostream>
#include <expected>
#include <list>

namespace core {

    class ResourceManager {
    public:
        static ResourceManager& instance();

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        [[nodiscard]] sf::Texture& load_texture(const std::string& name, const std::filesystem::path& path);
        [[nodiscard]] sf::Texture& get_texture(const std::string& name);
        [[nodiscard]] bool has_texture(const std::string& name) const;
        bool has_font(const std::string& name) const;
        bool has_sound_buffer(const std::string& name) const;
        
        void play_sound(const std::string& name);
        void clean_sounds(); // Call this occasionally to remove stopped sounds

        [[nodiscard]] sf::Font& load_font(const std::string& name, const std::filesystem::path& path);
        [[nodiscard]] sf::Font& get_font(const std::string& name);

        // Sound Buffer Management
        sf::SoundBuffer& load_sound_buffer(const std::string& name, const std::filesystem::path& path);
        sf::SoundBuffer& get_sound_buffer(const std::string& name);

    private:
        ResourceManager() = default;

        sf::Texture create_fallback_texture();

        std::unordered_map<std::string, sf::Texture> m_textures;
        std::unordered_map<std::string, sf::Font> m_fonts;
        std::unordered_map<std::string, sf::SoundBuffer> m_sound_buffers;
        std::list<sf::Sound> m_active_sounds;
    };

} // namespace core
