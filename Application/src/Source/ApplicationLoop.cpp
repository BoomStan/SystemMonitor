#include "../Headers/ApplicationLoop.h"
#include "../Headers/CpuInfo.h"
#include "../Headers/MemoryInfo.h"
#include "../Headers/NetworkInfo.h"
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
	monitors.push_back(std::make_unique<MemoryInfo>());
	monitors.push_back(std::make_unique<NetworkInfo>());
}

void Application::StartMonitors() {
	for (const auto& monitor : monitors) {
		monitor->Start();
	}
}

void Application::DrawMonitors()
{
	for (const auto& monitor : monitors) {
		monitor->Display();
	}
}

void Application::StopMonitors() 
{
for (const auto& monitor : monitors) {
		monitor->Stop();
	}
}

BackendGLFW& Application::GetBackendGLFW() 
{
	return backend_glfw;
}

BackendImGui& Application::GetBackendImGui() 
{
	return backend_imgui;
}

void Application::Start()
{
	//application Start goes here
	backend_glfw.Initialize(windowWidth, windowHeight, title);
	backend_imgui.Initialize(backend_glfw.GetWindow());

	MonitorsInit();
	StartMonitors();

}

void Application::EarlyUpdate() 
{
	backend_glfw.BeginGlfw();
	backend_imgui.ImGuiBegin();
}

void Application::Update() 
{
	backend_imgui.ImGuiDockingViewport();
	//application Update goes here
	DrawMonitors();

}

void Application::PostUpdate()
{
	backend_imgui.ImGuiEnd();
	backend_glfw.EndGlfw();
}


void Application::Stop() 
{
	//Application clean up or endings go here
	StopMonitors();
	backend_imgui.Cleanup();
	backend_glfw.Cleanup();
}