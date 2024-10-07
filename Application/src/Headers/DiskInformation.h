#pragma once
#include "MonitorBase.h"
#include <windows.h>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>
#include <imgui.h>
#include <mutex>
#include <windows.h> // For ULARGE_INTEGER
#include <iostream>



class DiskInformation : public MonitorBase {
public:
    // Struct to store disk data
    struct DiskData {
        uint64_t totalSpace = 0;
        uint64_t usedSpace = 0;
        uint64_t freeSpace = 0;
        uint64_t currentWrittenBytes = 0;
        uint64_t lastWrittenBytes = 0;
        float writeSpeedKBps = 0.0f;

        // Constructor to initialize all variables if needed (optional, as default member initializers are used)
        DiskData() = default;
    };
	void Display() override;
	void Start() override;
	void Stop() override;
private:
    // Private methods
    void DiskThreadFunction();   // Thread function for pulling disk data
    std::string FormatBytes(long long bytes);
    std::string ConvertWStringToString(const std::wstring& wstr); // Utility function to convert wstring to string

    // Variables
    std::vector<DiskData> diskDataList; // List of disk data (if you monitor multiple disks)
    std::vector<std::wstring> diskNames; // List of disk names (if needed)

    std::thread diskThread;  // Thread for disk monitoring
    bool running = false;    // Flag to control the thread
};

