#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


class BackendImGui {
private: 


public:

    BackendImGui();
    ~BackendImGui();

    bool Initialize(GLFWwindow* window);
    void ImGuiRedraw();
    void Cleanup();
};