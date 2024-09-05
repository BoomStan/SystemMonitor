#include "../Headers/ApplicationLoop.h"
#include "../Headers/BackendGLFW.h"
#include "../Headers/BackendImGui.h"
// Callback function to adjust viewport when the window size changes
Application app;

int main() 
{
	app.Start();

	while (!glfwWindowShouldClose(app.GetBackendGLFW().GetWindow()) )
	{
		app.Update();
	}

	app.Stop();
	return 0;
}