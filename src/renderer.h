#ifndef N_BODY_SIMULATION_GL_RENDERER_H
#define N_BODY_SIMULATION_GL_RENDERER_H
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "camera.h"
#include "body.h"
#include "shader.h"

class renderer {
public:
    renderer(int width, int height, const char *title);

    ~renderer();

    bool init();

    void setupBuffers(const SphereData &sphereData, unsigned int numBodies);

    void renderFrame(const std::vector<body> &bodies, const Shader &shader);

    [[nodiscard]] bool shouldClose() const;

    void processInput(double deltaTime);

    void swapBuffers() const;

    [[nodiscard]] Camera &getCamera() { return camera; };
    [[nodiscard]] GLFWwindow *getWindow() const { return window; };

private:
    int width, height;
    const char *title;
    GLFWwindow *window;
    Camera camera;
    unsigned int VAO, VBO, EBO, instanceVBO;
    size_t indexCount;

    bool menuMode;
    bool firstMouse;
    bool tabPressed;
    bool pausePressed;
    float lastX, lastY;

    static renderer *getRenderer(GLFWwindow *window);

    static void framebufferSizeCallback(GLFWwindow *window, int width, int height);

    static void mouseCallback(GLFWwindow *window, double xpos, double ypos);

    static void scrollCallback(GLFWwindow *window, double xoffset, double ypos);
};


#endif //N_BODY_SIMULATION_GL_RENDERER_H
