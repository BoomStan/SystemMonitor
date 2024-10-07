#pragma once
#include "../Headers/BackendImGui.h"

class MonitorBase {
public:
	virtual ~MonitorBase() {};
	virtual void Start() = 0;
	virtual void Display() = 0;
	virtual void Stop() = 0;
private:

};