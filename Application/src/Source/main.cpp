#include "../Headers/ApplicationLoop.h"
#include "../Headers/BackendGLFW.h"
#include "../Headers/BackendImGui.h"

Application app;

int main() 
{
	app.Start();

	while (!glfwWindowShouldClose(app.GetBackendGLFW().GetWindow()) )
	{
		app.EarlyUpdate();
		app.Update();
		app.PostUpdate();
	}

	app.Stop();
	return 0;
}