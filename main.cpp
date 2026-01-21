#include "renderer.h"
#include "shader.h"
#include "menuGUI.h"
#include "physicsEngine.h"
#include "config.h"


double deltaTime = 0.0f;
double lastFrame = 0.0f;

int main() {
    renderer renderEngine(CONFIG.screenWidth, CONFIG.screenHeight, CONFIG.windowTitle);
    menuGUI menu(renderEngine.getWindow());
    Shader shader("../shader.vert", "../shader.frag");


    auto bodies = body::generateBodies(menu.targetBodyCount);
    auto sphereData = body::generateSphereVertices(1.0f, 32);

    renderEngine.setupBuffers(sphereData, menu.targetBodyCount);

    while (!renderEngine.shouldClose()) {
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        renderEngine.processInput(deltaTime);

        if (menu.needsUpdate) {
            menu.update();
            bodies = body::generateBodies(CONFIG.numBodies);
            renderEngine.setupBuffers(sphereData, CONFIG.numBodies);
            menu.needsUpdate = false;
        }
        if (menu.needsReset) {
            menu.reset();
            menu.update();
            bodies = body::generateBodies(CONFIG.numBodies);
            renderEngine.setupBuffers(sphereData, CONFIG.numBodies);
            menu.needsReset = false;
        }
        if (!CONFIG.paused) {
            physicsEngine::update(bodies, deltaTime);
        }

        renderEngine.renderFrame(bodies, shader);
        menuGUI::newFrame();
        menu.render();
        renderEngine.swapBuffers();
    }
    return 0;
}



