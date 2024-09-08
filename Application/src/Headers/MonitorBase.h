#include "../Headers/BackendImGui.h"

#pragma once

class MonitorBase {
public:
	virtual ~MonitorBase() {};
	virtual void Start() = 0;
	virtual void Display() = 0;

};