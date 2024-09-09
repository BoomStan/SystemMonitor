#include "MonitorBase.h"
#include <imgui.h>
#include <windows.h>
#include <vector>
#include <iostream>
#include <pdh.h>
#include <string>
#include <thread>
#include <mutex>

class CpuInfo : public MonitorBase {
public:
	void Start() override;
	void Display() override;
	void Stop() override;
private:
	void ThreadFunc(int index); 
	float GetTotalCpuUsage();
	int GetTotalCores();
	float PollGetCoreUsage(int index);
};
