#include "renderer.h"
#include "shader.h"
#include "menuGUI.h"
#include "physicsEngine.h"

constexpr unsigned int SCR_WIDTH = 1980;
constexpr unsigned int SCR_HEIGHT = 1080;
unsigned int numBodies = 1;

// Timing
double deltaTime = 0.0f;
double lastFrame = 0.0f;

int main() {
    renderer renderEngine(SCR_WIDTH, SCR_HEIGHT, "N-Body Simulation OpenGL");
    menuGUI menu(renderEngine.getWindow());
    Shader shader("../shader.vert", "../shader.frag");


    auto bodies = body::generateBodies(numBodies);
    auto sphereData = body::generateSphereVertices(1.0f, 32);

    renderEngine.setupBuffers(sphereData, numBodies);

    while (!renderEngine.shouldClose()) {
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        renderEngine.processInput(deltaTime);
        physicsEngine::update(bodies, deltaTime);
        renderEngine.renderFrame(bodies, shader);
        menuGUI::newFrame();
        menu.render();
        renderEngine.swapBuffers();
    }
    return 0;
}



