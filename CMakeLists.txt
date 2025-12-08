cmake_minimum_required(VERSION 3.10)
project(game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add libraries
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
# find_package(OpenMP REQUIRED)
# find_package(Freetype CONFIG REQUIRED)
find_package(soil2 CONFIG REQUIRED)

# Collect sources from all modules
file(GLOB_RECURSE ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/engine/src/*.cpp")
file(GLOB_RECURSE GAME_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/game/src/*.cpp")

add_executable(game
    ${ENGINE_SOURCES}
    ${GAME_SOURCES}
)

target_include_directories(game PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/include
    ${CMAKE_CURRENT_SOURCE_DIR}/game/include
)

target_link_libraries(game PUBLIC
    glfw
    glm::glm
    glad::glad
    assimp::assimp
    imgui::imgui
    soil2
    # Freetype::Freetype
)
