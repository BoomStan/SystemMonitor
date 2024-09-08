#include "MonitorBase.h"
#include <imgui.h>

class CpuInfo : public MonitorBase {
public:
	void Start() override;
	void Display() override;
private:
	float GetTotalCpuUsage();
	int GetTotalCores();
	float GetCoreUsage(int index);
};
