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
     menuGUI(GLFWwindow* window);
    ~menuGUI();
    void render();
    static void newFrame();

    int targetBodyCount = 1000;
    bool needsUpdate = false;

private:
    GLFWwindow* window{};
};


#endif //N_BODY_SIMULATION_GL_MENUGUI_H