cmake_minimum_required(VERSION 3.10)
project(OPENGL_APP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add libralies
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
# find_package(OpenMP REQUIRED)
# find_package(Freetype CONFIG REQUIRED)
# find_package(soil2 CONFIG REQUIRED)

file(GLOB_RECURSE OPENGLAPP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(OPENGL_APP ${OPENGLAPP_SOURCES}) # ${EXTRA_SOURCES}

target_include_directories(OPENGL_APP PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

target_link_libraries(OPENGL_APP PUBLIC
    glfw
    glm::glm
    glad::glad
    assimp::assimp
    imgui::imgui
    # soil2
    # Freetype::Freetype
)
