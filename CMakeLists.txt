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

# ------------------------------------
# Fetch ImGui
# ------------------------------------
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.91.5  # Use a specific stable version
)

# Download dependencies
FetchContent_MakeAvailable(glfw glm imgui)

# ------------------------------------
# GLAD library
# ------------------------------------
add_library(glad ${EXTERNAL_DIR}/GLAD/src/glad.c)
target_include_directories(glad PUBLIC ${EXTERNAL_DIR}/GLAD/include)

# ------------------------------------
# ImGui library
# ------------------------------------
add_library(imgui_lib
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_lib PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_lib PUBLIC glfw glad)

# ------------------------------------
# n_body_simulation_GL executable
# ------------------------------------
add_executable(n_body_simulation_GL
        main.cpp
        shader.cpp
        shader.h
        body.cpp
        body.h
        camera.cpp
        camera.h
        menuGUI.cpp
        menuGUI.h
        renderer.cpp
        renderer.h
        physicsEngine.cpp
        physicsEngine.h
        config.h
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
        imgui_lib
)

# ------------------------------------
# Include directories
# ------------------------------------
target_include_directories(n_body_simulation_GL PRIVATE
        ${EXTERNAL_DIR}/GLAD/include
)