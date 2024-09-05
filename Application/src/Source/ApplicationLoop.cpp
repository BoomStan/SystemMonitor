#include "../Headers/ApplicationLoop.h"

int windowWidth = 800;            // Desired width of the window
int windowHeight = 600;           // Desired height of the window

const char* title = "My Application"; // Title of the window

BackendGLFW backend_glfw;
BackendImGui backend_imgui;

Application::Application() 
{

}

Application::~Application() 
{
	
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
}

void Application::Update() 
{
	//application Update goes here

	backend_glfw.BeginGlfw();
	backend_imgui.ImGuiRedraw();
	backend_glfw.EndGlfw();
}



void Application::Stop() 
{
	//Application clean up or endings go here
	backend_imgui.Cleanup();
	backend_glfw.Cleanup();
}