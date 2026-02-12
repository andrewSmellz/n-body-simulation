#include "renderer.h"
#include "shader.h"
#include "menuGUI.h"
#include "physicsEngine.h"
#include "config.h"


int main() {
    renderer renderEngine(config.screenWidth, config.screenHeight, config.windowTitle);
    menuGUI menu(renderEngine.getWindow());
    Shader shader("shaders/shader.vert", "shaders/shader.frag");
    auto bodies = body::generateBodies(menu.targetBodyCount);
    auto sphereData = body::generateSphereVertices(1.0f, 32);

    renderEngine.setupBuffers(sphereData, menu.targetBodyCount);

    double deltaTime = 0.0f;
    double lastFrame = 0.0f;
    double currentTime;


    while (!renderEngine.shouldClose()) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        renderEngine.processInput(deltaTime);

        if (menu.needsUpdate) {
            menu.update();
            bodies = body::generateBodies(config.numBodies);
            renderEngine.setupBuffers(sphereData, config.numBodies);
            menu.needsUpdate = false;
        }
        if (menu.needsReset) {
            menu.reset();
            menu.update();
            bodies = body::generateBodies(config.numBodies);
            renderEngine.setupBuffers(sphereData, config.numBodies);
            menu.needsReset = false;
        }
        if (!config.paused) {
            physicsEngine::update(bodies, deltaTime);
        }

        renderEngine.renderFrame(bodies, shader);
        menuGUI::newFrame();
        menu.render();
        renderEngine.swapBuffers();
    }
    return 0;
}



