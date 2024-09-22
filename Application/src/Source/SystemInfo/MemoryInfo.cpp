#include "../../Headers/MemoryInfo.h"

// Define the structure for process memory information
struct MemoryInfo::ProcessMemoryInfo {
    DWORD processID;
    std::string processName;
    SIZE_T memoryUsage;
};

void MemoryInfo::Start() { }

void MemoryInfo::Display()
{
    // Get system memory info
    MEMORYSTATUSEX memInfo;
    getSystemMemoryInfo(memInfo);

    // Get list of top memory-consuming processes
    std::vector<ProcessMemoryInfo> topProcesses = MemoryInfo::getTopMemoryProcesses();

    // Create ImGui window for system memory information
    ImGui::Begin("System Memory Information");

    ImGui::Text("Total physical memory: %.2f MB", memInfo.ullTotalPhys / (1024.0 * 1024.0));
    ImGui::Text("Available physical memory: %.2f MB", memInfo.ullAvailPhys / (1024.0 * 1024.0));
    ImGui::Text("Used physical memory: %.2f MB",
        (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024.0 * 1024.0));
    ImGui::Text("Memory in use: %lu %%", memInfo.dwMemoryLoad);

    ImGui::Separator(); // A line separator in ImGui

    ImGui::Text("Top Processes by Memory Usage:");

    // Create a list in ImGui to display process memory information
    if (ImGui::BeginListBox("##process_list", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing()))) {
        for (size_t i = 0; i < std::min(topProcesses.size(), static_cast<size_t>(10)); ++i) {
            const ProcessMemoryInfo& process = topProcesses[i];
            ImGui::Text("PID: %lu - Memory: %.2f MB - Name: %s", process.processID, process.memoryUsage / (1024.0 * 1024.0), process.processName.c_str());
        }
        ImGui::EndListBox();
    }

    ImGui::End();
}

void MemoryInfo::Stop() { }

void MemoryInfo::getSystemMemoryInfo(MEMORYSTATUSEX& memInfo)
{
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
}

// Function to get memory usage of a specific process
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


    //release the handle to avoid resource leaks
    CloseHandle(hProcess);
    return 0;
}

// Helper function to retrieve process name
std::string MemoryInfo::getProcessName(DWORD processID) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    //null check
    if (hProcess == NULL) {
        return "Unknown";
    }

    //initialize character array
    char processName[MAX_PATH] = "<unknown>";
    HMODULE hMod;
    DWORD cbNeeded;

    //return the process name
    if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
        GetModuleBaseNameA(hProcess, hMod, processName, sizeof(processName) / sizeof(char));
    }

    //release the handle to avoid resource leaks
    CloseHandle(hProcess);
    return std::string(processName);
}

// Function to get top memory-consuming processes
std::vector<MemoryInfo::ProcessMemoryInfo> MemoryInfo::getTopMemoryProcesses() {
    DWORD processes[1024], cbNeeded, cProcesses;
    std::vector<ProcessMemoryInfo> processMemoryList;

    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
        return processMemoryList;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    //loops throughout every process
    for (unsigned int i = 0; i < cProcesses; i++) {
        if (processes[i] != 0) {
            SIZE_T memoryUsage = getProcessMemoryUsage(processes[i]);
            if (memoryUsage > 0) {
                std::string processName = getProcessName(processes[i]);
                processMemoryList.push_back({ processes[i], processName, memoryUsage });
            }
        }
    }

    //Sort based on memory usage
    std::sort(processMemoryList.begin(), processMemoryList.end(),
        [](const ProcessMemoryInfo& a, const ProcessMemoryInfo& b) {
            return a.memoryUsage > b.memoryUsage;
        });

    return processMemoryList;
}

//TODO: add thread for pulling the memory information