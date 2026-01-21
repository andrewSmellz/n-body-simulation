//
// Created by andre on 2026-01-21.
//

#include "menuGUI.h"
#include "config.h"

menuGUI::menuGUI(GLFWwindow *window) : window(window), targetBodyCount(CONFIG.numBodies) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");
    ImGui::StyleColorsDark();
}

menuGUI::~menuGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void menuGUI::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void menuGUI::render() {
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    ImVec2 menuPos(display_w - 375, display_h - 475);
    ImGui::SetNextWindowPos(menuPos);
    ImGui::SetNextWindowBgAlpha(0.75f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoFocusOnAppearing |
                             ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("controls", nullptr, flags)) {
        ImGui::Text("N-Body Simulation");
        ImGui::Separator();
        ImGui::Text("TAB to swap between mouse and camera");
        ImGui::Text("P to pause/unpause simulation");
        ImGui::Separator();

        ImGui::Text("Body Count");
        ImGui::InputInt("##BodyCount", &targetBodyCount, 1, 100);
        if (targetBodyCount < 0) targetBodyCount = 0;

        ImGui::Separator();
        ImGui::Text("Physics Settings");
        ImGui::InputFloat("Gravitational Constant", &targetGravitationalConstant, 100.0f, 1000.0f);
        ImGui::InputFloat("timescale", &targetTimeScale, 0.1f, 10.0f);


        ImGui::Separator();
        ImGui::Text("Central Body");
        ImGui::InputFloat("Mass##Central", &targetCentralBodyMass, 1000.0f, 50000.0f);
        ImGui::InputFloat("Radius##Central", &targetCentralBodyRadius, 1.0f, 50.0f);

        ImGui::Separator();
        ImGui::Text("Orbit Radius Range");
        ImGui::InputFloat("Min##Orbit", &targetMinOrbitRadius, 50.0f, 500.0f);
        ImGui::InputFloat("Max##Orbit", &targetMaxOrbitRadius, 100.0f, 800.0f);
        if (targetMaxOrbitRadius < targetMinOrbitRadius) {
            targetMaxOrbitRadius = targetMinOrbitRadius;
        }

        ImGui::Separator();
        ImGui::Text("Body Mass Range");
        ImGui::InputFloat("Min##Mass", &targetMinBodyMass, 0.1f, 10.0f);
        ImGui::InputFloat("Max##Mass", &targetMaxBodyMass, 0.1f, 100.0f);
        if (targetMaxBodyMass < targetMinBodyMass) {
            targetMaxBodyMass = targetMinBodyMass;
        }

        ImGui::Separator();
        ImGui::Text("Body Radius Range");
        ImGui::InputFloat("Min##Radius", &targetMinBodyRadius, 1.0f, 10.0f);
        ImGui::InputFloat("Max##Radius", &targetMaxBodyRadius, 1.0f, 50.0f);
        if (targetMaxBodyRadius < targetMinBodyRadius) {
            targetMaxBodyRadius = targetMinBodyRadius;
        }

        ImGui::Separator();
        if (ImGui::Button("Apply Changes")) {
            needsUpdate = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Simulation")) {
            needsReset = true;
        }

        ImGui::Separator();
        ImGui::Text("Application average %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void menuGUI::update() const {
    CONFIG.numBodies = targetBodyCount;
    CONFIG.gravitationalConstant = targetGravitationalConstant;
    CONFIG.timeScale = targetTimeScale;
    CONFIG.centralBodyMass = targetCentralBodyMass;
    CONFIG.centralBodyRadius = targetCentralBodyRadius;
    CONFIG.minOrbitRadius = targetMinOrbitRadius;
    CONFIG.maxOrbitRadius = targetMaxOrbitRadius;
    CONFIG.minBodyMass = targetMinBodyMass;
    CONFIG.maxBodyMass = targetMaxBodyMass;
    CONFIG.minBodyRadius = targetMinBodyRadius;
    CONFIG.maxBodyRadius = targetMaxBodyRadius;
}

void menuGUI::reset() {
    targetBodyCount = 1;
    targetGravitationalConstant = 1000.0f;
    targetTimeScale = 1.0f;
    targetCentralBodyMass = 10000.0f;
    targetCentralBodyRadius = 100.0f;
    targetMinOrbitRadius = 170.0f;
    targetMaxOrbitRadius = 400.0f;
    targetMinBodyMass = 1.0f;
    targetMaxBodyMass = 10.0f;
    targetMinBodyRadius = 15.0f;
    targetMaxBodyRadius = 30.0f;
}
