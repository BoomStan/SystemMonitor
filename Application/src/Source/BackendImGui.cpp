#include "../Headers/BackendImGui.h"

bool show_demo_window = false;

// Constructor
BackendImGui::BackendImGui() 
{

}

// Destructor
BackendImGui::~BackendImGui() 
{

}

// Initialize ImGui
bool BackendImGui::Initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    GetImGuiIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    GetImGuiIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    GetImGuiIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    GetImGuiIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return true;
}

ImGuiIO& BackendImGui::GetImGuiIO()
{
    return ImGui::GetIO(); (void)GetImGuiIO();
}

ImGuiStyle& BackendImGui::GetImGuiStyle() 
{
    return ImGui::GetStyle();
}

void BackendImGui::ImGuiBegin() 
{
    GetImGuiIO() = ImGui::GetIO(); (void)GetImGuiIO();

    // Start a new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void BackendImGui::ImGuiDockingViewport() 
{    
    // ImGui rendering logic goes here, for example:
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);
    GetImGuiStyle().Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.06, 0.06, 0.06, 1.0);
    GetImGuiStyle().Colors[ImGuiCol_DockingPreview] = ImVec4(0.15, 0.17, 1.00, 1.0);
}

void BackendImGui::ImGuiEnd() 
{
    // Render ImGui
    ImGui::Render();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Handle multiple viewports if enabled
    if (GetImGuiIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void BackendImGui::Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}