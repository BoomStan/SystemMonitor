#pragma once
#define NOMINMAX
#include <windows.h>
#include "MonitorBase.h"
#include <imgui.h>
#include <algorithm>
#include <vector>
#include <psapi.h>
#include <iostream>


class MemoryInfo : public MonitorBase
{
public:
	struct ProcessMemoryInfo;
	void Start();
	void Display();
	void Stop();
	std::vector<ProcessMemoryInfo> getTopMemoryProcesses();
private:
	void getSystemMemoryInfo(MEMORYSTATUSEX& memInfo);
	SIZE_T getProcessMemoryUsage(DWORD processID);
	std::string getProcessName(DWORD processID);

};