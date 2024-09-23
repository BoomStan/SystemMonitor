#include "../../Headers/MemoryInfo.h"

// Define the structure for process memory information


MemoryInfo::MemoryInfo() : stopThread(false) {}

void MemoryInfo::Start() {
    // Start the background thread to periodically update process memory info
    stopThread = false;
    updateTopThread = std::thread(&MemoryInfo::UpdateTopProcessMemoryInfo, this);
    updateMemoryThread = std::thread(&MemoryInfo::UpdateMemoryInfo, this);
}

struct MemoryInfo::ProcessMemoryInfo {
    DWORD processID;
    std::string processName;
    SIZE_T memoryUsage;
};


void MemoryInfo::Display() {
    // Get system memory info
    

    // Create ImGui window for system memory information
    ImGui::Begin("System Memory Information");
    std::lock_guard<std::mutex> guard(processMemoryInfo);
    ImGui::Text("Total physical memory: %.2f MB", memoryInfo.ullTotalPhys / (1024.0 * 1024.0));
    ImGui::Text("Available physical memory: %.2f MB", memoryInfo.ullAvailPhys / (1024.0 * 1024.0));
    ImGui::Text("Used physical memory: %.2f MB",
        (memoryInfo.ullTotalPhys - memoryInfo.ullAvailPhys) / (1024.0 * 1024.0));
    ImGui::Text("Memory in use: %lu %%", memoryInfo.dwMemoryLoad);

    ImGui::Separator(); // A line separator in ImGui

    ImGui::Text("Top Processes by Memory Usage:");

    // Lock the mutex to safely read the shared topProcesses data
    {
        std::lock_guard<std::mutex> guard(processTopMemoryMutex);
        if (ImGui::BeginListBox("##process_list", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {
            for (size_t i = 0; i < std::min(topProcesses.size(), static_cast<size_t>(10)); ++i) {
                const ProcessMemoryInfo& process = topProcesses[i];
                ImGui::Text("PID: %lu - Memory: %.2f MB", process.processID, process.memoryUsage / (1024.0 * 1024.0));
            }
            ImGui::EndListBox();
        }
    }

    ImGui::End();
}

void MemoryInfo::Stop() {
    // Stop the background thread
    stopThread = true;
    if (updateTopThread.joinable()) {
        updateTopThread.join();
    }
    if (updateMemoryThread.joinable()) {
        updateMemoryThread.join();
    }

}

// Background thread function that updates the process memory info
void MemoryInfo::UpdateTopProcessMemoryInfo() {
    while (!stopThread) {
        std::vector<ProcessMemoryInfo> updatedProcesses = getTopMemoryProcesses();

        // Lock the mutex to safely update the shared topProcesses data
        {
            std::lock_guard<std::mutex> guard(processTopMemoryMutex);
            topProcesses = updatedProcesses;
        }

        // Sleep for 500ms before updating again
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void MemoryInfo::UpdateMemoryInfo() 
{
    while (!stopThread) 
    {
        MEMORYSTATUSEX updatedMemoryInfo;
        MemoryInfo::getSystemMemoryInfo(updatedMemoryInfo);

        {
            std::lock_guard<std::mutex> guard(processMemoryInfo);
            memoryInfo = updatedMemoryInfo;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Other utility functions...

void MemoryInfo::getSystemMemoryInfo(MEMORYSTATUSEX& memInfo) {
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
}

SIZE_T MemoryInfo::getProcessMemoryUsage(DWORD processID) {
    PROCESS_MEMORY_COUNTERS pmc;
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    if (hProcess == NULL) {
        return 0;
    }

    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        CloseHandle(hProcess);
        return pmc.WorkingSetSize; // Memory usage in bytes
    }

    CloseHandle(hProcess);
    return 0;
}

std::string MemoryInfo::getProcessName(DWORD processID) {
    char processName[MAX_PATH] = "<unknown>";
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    if (hProcess != NULL) {
        HMODULE hMod;
        DWORD cbNeeded;
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            GetModuleBaseNameA(hProcess, hMod, processName, sizeof(processName) / sizeof(char));
        }
        CloseHandle(hProcess);
    }

    return std::string(processName);
}

std::vector<MemoryInfo::ProcessMemoryInfo> MemoryInfo::getTopMemoryProcesses() {
    DWORD processes[1024], cbNeeded, cProcesses;
    std::vector<ProcessMemoryInfo> processMemoryList;

    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
        return processMemoryList;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    for (unsigned int i = 0; i < cProcesses; i++) {
        if (processes[i] != 0) {
            SIZE_T memoryUsage = MemoryInfo::getProcessMemoryUsage(processes[i]);
            if (memoryUsage > 0) {
                std::string processName = MemoryInfo::getProcessName(processes[i]);
                processMemoryList.push_back({ processes[i], processName, memoryUsage });
            }
        }
    }

    std::sort(processMemoryList.begin(), processMemoryList.end(),
        [](const ProcessMemoryInfo& a, const ProcessMemoryInfo& b) {
            return a.memoryUsage > b.memoryUsage;
        });

    return processMemoryList;
}