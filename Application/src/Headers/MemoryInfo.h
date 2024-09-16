#pragma once
#include "MonitorBase.h"
#include <imgui.h>

class MemoryInfo : public MonitorBase
{
public:
	void Start();
	void Display();
	void Stop();

};