#pragma once
// Define WIN32_LEAN_AND_MEAN to exclude rarely-used services from Windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>    // Include Windows header
#include <Pdh.h>        // Include PDH for performance data
#include <iostream>
#include <imgui.h>
#include <chrono>
#include <thread>
#include <atomic>
#include "MonitorBase.h"
#include <string>

class NetworkInfo : public MonitorBase {
public:
    void Start() override;
    void Display() override;
    void Stop() override;
     
private:
    long long lastBytesSent = 0;
    long long lastBytesRecv = 0;
    long long uploadSpeed = 0;
    long long downloadSpeed = 0;
    std::thread networkThread;

    std::string ConvertWStringToString(const std::wstring& wstr);
    void UpdateNetworkData();
    void NetworkThreadFunction();
};