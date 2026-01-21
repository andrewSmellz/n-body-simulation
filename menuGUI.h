//
// Created by andre on 2026-01-21.
//

#ifndef N_BODY_SIMULATION_GL_MENUGUI_H
#define N_BODY_SIMULATION_GL_MENUGUI_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

class menuGUI {
public:
    menuGUI(GLFWwindow *window);

    ~menuGUI();

    void render();

    static void newFrame();

    void update() const;

    void reset();

    int targetBodyCount = 1;
    float targetGravitationalConstant = 1000.0f;
    float targetTimeScale = 1.0f;
    float targetCentralBodyMass = 10000.0f;
    float targetCentralBodyRadius = 100.0f;
    float targetMinOrbitRadius = 170.0f;
    float targetMaxOrbitRadius = 400.0f;
    float targetMinBodyMass = 1.0f;
    float targetMaxBodyMass = 10.0f;
    float targetMinBodyRadius = 15.0f;
    float targetMaxBodyRadius = 30.0f;
    bool needsUpdate = false;
    bool needsReset = false;

private:
    GLFWwindow *window{};
};


#endif //N_BODY_SIMULATION_GL_MENUGUI_H
