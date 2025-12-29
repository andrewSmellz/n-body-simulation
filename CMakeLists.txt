cmake_minimum_required(VERSION 3.20)
project(n_body_simulation_GL VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ------------------------------------
# External directory for dependencies
# ------------------------------------
set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external")

# ------------------------------------
# Fetch GLFW
# ------------------------------------
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

# ------------------------------------
# Fetch GLM
# ------------------------------------
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)

# Download dependencies
FetchContent_MakeAvailable(glfw glm)

# ------------------------------------
# GLAD library
# ------------------------------------
add_library(glad ${EXTERNAL_DIR}/GLAD/src/glad.c)
target_include_directories(glad PUBLIC ${EXTERNAL_DIR}/GLAD/include)

# ------------------------------------
# n_body_simulation_GL executable
# ------------------------------------
add_executable(n_body_simulation_GL
        main.cpp
        shader.cpp
        shader.h
        body.cpp
        body.h
)

# ------------------------------------
# Find OpenGL
# ------------------------------------
find_package(OpenGL REQUIRED)

# ------------------------------------
# Linking
# ------------------------------------
target_link_libraries(n_body_simulation_GL
        glad
        glfw
        glm::glm
        OpenGL::GL
)

# ------------------------------------
# Include directories
# ------------------------------------
target_include_directories(n_body_simulation_GL PRIVATE
        ${EXTERNAL_DIR}/GLAD/include
)