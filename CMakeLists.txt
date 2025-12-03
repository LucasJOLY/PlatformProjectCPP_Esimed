cmake_minimum_required(VERSION 3.15)
project(PlatformProjectCPP_Esimed VERSION 1.0.0 LANGUAGES CXX)

# --- C++ Standard Settings ---
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Output Directory ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- SFML Configuration ---
# Hint for Homebrew on Apple Silicon
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

find_package(SFML 3.0 COMPONENTS Graphics Window System Audio REQUIRED)

# --- Source Files ---
# Note: GLOB_RECURSE is used, so touch this file if adding new sources (Enemy added)
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.hpp"
    "src/core/ResourceManager.cpp"
    "src/core/GameWindow.cpp"
    "src/core/LevelProgress.cpp"
    "src/core/SkinManager.cpp"
    "src/states/StateManager.cpp"
    "src/states/GameState.cpp"
    "src/states/MainMenuState.cpp"
    "src/states/LevelSelectionState.cpp"
    "src/states/SkinSelectionState.cpp"
    "src/entities/Entity.cpp"
)

# --- Executable ---
add_executable(${PROJECT_NAME} ${SOURCES})

# --- Include Directories ---
target_include_directories(${PROJECT_NAME} PRIVATE src)

# --- Link Libraries ---
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)

# --- Compiler Warnings (Optional but recommended) ---
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- Copy Assets (if we had them, placeholder for now) ---
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_SOURCE_DIR}/assets
#     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
