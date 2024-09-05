#include "../Headers/BackendGLFW.h"
#include "../Headers/BackendImGui.h"

#pragma once

class Application {

public:
	Application();
	~Application();
	BackendGLFW& GetBackendGLFW();
	BackendImGui& GetBackendImGui();
	void Start();
	void Update();
	void Stop();
};