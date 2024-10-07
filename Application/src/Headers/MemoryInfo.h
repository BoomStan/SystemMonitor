#pragma once
#define NOMINMAX
#include <windows.h>
#include "MonitorBase.h"
#include <imgui.h>
#include <algorithm>
#include <vector>
#include <psapi.h>
#include <iostream>
#include <mutex>          // For std::mutex
#include <thread>         // For std::thread
#include <atomic> 
#include <iphlpapi.h>




class MemoryInfo : public MonitorBase
{

public:
    MemoryInfo();
	struct ProcessMemoryInfo;
    void Start() override;
    void Display() override;
    void Stop() override;

private:
    void getSystemMemoryInfo(MEMORYSTATUSEX& memInfo);
    SIZE_T getProcessMemoryUsage(DWORD processID);
    std::string getProcessName(DWORD processID);
    void UpdateTopProcessMemoryInfo();  // Thread function
    void UpdateMemoryInfo();

    std::vector<ProcessMemoryInfo> getTopMemoryProcesses();

    std::vector<ProcessMemoryInfo> topProcesses;  // Shared memory for the process info
    MEMORYSTATUSEX memoryInfo;
    std::thread updateTopThread;                     // Background thread
    std::thread updateMemoryThread;
    std::mutex processTopMemoryMutex;                      // Mutex for thread synchronization
    std::mutex processMemoryInfo;
    std::atomic<bool> stopThread;                 // Flag to stop the thread
};
