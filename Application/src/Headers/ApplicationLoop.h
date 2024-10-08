#include "../Headers/BackendGLFW.h"
#include "../Headers/BackendImGui.h"

#pragma once

class Application {
private:
	void MonitorsInit();
	void DrawMonitors();
	void StartMonitors();
	void StopMonitors();
public:
	Application();
	~Application();
	BackendGLFW& GetBackendGLFW();
	BackendImGui& GetBackendImGui();
	void Start();
	void EarlyUpdate();
	void Update();
	void PostUpdate();
	void Stop();
};