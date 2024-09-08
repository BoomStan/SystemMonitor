#include "../Headers/ApplicationLoop.h"
#include "../Headers/CpuInfo.h"
#include <vector>
#include <memory>

int windowWidth = 800;            // Desired width of the window
int windowHeight = 600;           // Desired height of the window

const char* title = "My Application"; // Title of the window

BackendGLFW backend_glfw;
BackendImGui backend_imgui;

std::vector<std::unique_ptr<MonitorBase>> monitors;

Application::Application() 
{

}

Application::~Application() 
{
	
}

void Application::MonitorsInit() 
{
	monitors.push_back(std::make_unique<CpuInfo>());
}

void Application::DrawMonitors()
{
	for (const auto& monitor : monitors) {
		monitor->Display();
	}
}

BackendGLFW& Application::GetBackendGLFW() {
	return backend_glfw;
}

BackendImGui& Application::GetBackendImGui() {
	return backend_imgui;
}

void Application::Start()
{
	//application Start goes here
	backend_glfw.Initialize(windowWidth, windowHeight, title);
	backend_imgui.Initialize(backend_glfw.GetWindow());
	MonitorsInit();
}

void Application::EarlyUpdate() 
{
	backend_glfw.BeginGlfw();
	backend_imgui.ImGuiBegin();
}

void Application::Update() 
{
	//application Update goes here
	DrawMonitors();
	backend_imgui.ImGuiRedraw();
}

void Application::PostUpdate()
{
	backend_imgui.ImGuiEnd();
	backend_glfw.EndGlfw();
}


void Application::Stop() 
{
	//Application clean up or endings go here
	backend_imgui.Cleanup();
	backend_glfw.Cleanup();
}