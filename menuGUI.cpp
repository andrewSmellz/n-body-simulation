//
// Created by andre on 2026-01-21.
//

#include "menuGUI.h"

menuGUI::menuGUI(GLFWwindow *window):window(window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
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
    ImVec2 menuPos(display_w-275, display_h-165);
    ImGui::SetNextWindowPos(menuPos);
    ImGui::SetNextWindowBgAlpha(0.75f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                    ImGuiWindowFlags_AlwaysAutoResize |
                                    ImGuiWindowFlags_NoSavedSettings |
                                    ImGuiWindowFlags_NoFocusOnAppearing |
                                    ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("controls",nullptr,flags)) {
        ImGui::Text("N-Body Simulation");
        ImGui::Separator();
        ImGui::Text("TAB to swap between mouse and camera");
        ImGui::Separator();

        ImGui::Text("Body Count");
        int prevCount = targetBodyCount;
        ImGui::SliderInt("##bodycount", &targetBodyCount, 1, 5000);

        if (ImGui::Button("Apply Changes")) {
            needsUpdate = true;
        }

        ImGui::Separator();
        ImGui::Text("Application average %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
