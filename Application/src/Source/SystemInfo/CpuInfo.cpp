#include "../../Headers/CpuInfo.h"

ImGuiWindowFlags_ flags;
std::vector<float> coreUsages; // To store CPU usage values
std::vector<std::thread> threads; // To store thread objects
std::mutex usageMutex; // To protect access to coreUsages
int coreCount = 0; // Number of cores
std::atomic<bool> shouldTerminate{false}; // Signal to stop threads

void CpuInfo::Start() {
    flags = ImGuiWindowFlags_NoCollapse;  // Correct assignment

    coreCount = GetTotalCores();
    coreUsages.resize(coreCount);

    // Create a thread for each core
    shouldTerminate = false; // Ensure the termination flag is false when starting
    for (int i = 0; i < coreCount; ++i) {
        threads.emplace_back(&CpuInfo::ThreadFunc, this, i);
    }
}

void CpuInfo::ThreadFunc(int index) {
    while (!shouldTerminate) {  // Continue polling until the application is stopped
        float usage = PollGetCoreUsage(index);
        {
            std::lock_guard<std::mutex> lock(usageMutex);  // Lock the coreUsages access
            coreUsages[index] = usage;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Sleep between polling
    }
}

void CpuInfo::Display() 
{
    ImGui::Begin("CPU", nullptr); // Main window

    // Begin child frame with size (optional), with or without border and scroll options
    ImGui::BeginChild("Cpu Usage", ImVec2(0, 0), true); // Child window with a specific height (300px)

    for (int i = 0; i < coreCount; i++) {
        // Core label and progress bar for each core
        std::string coreLabel = "Core " + std::to_string(i) + " Usage: ";
        ImGui::Text("%s %.2f%%", coreLabel.c_str(), coreUsages[i]);
        ImGui::ProgressBar(coreUsages[i] / 100.0f, ImVec2(-FLT_MIN, 0));  // FLT_MIN makes it take the full width of the window
    }
    ImGui::EndChild();  // End the child frame
    ImGui::End();  // End the main window
}

void CpuInfo::Stop() {
    shouldTerminate = true;  // Set to stop all threads

    // Join all threads
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    threads.clear();  // Clean up thread list
}

float CpuInfo::GetTotalCpuUsage() {
    return -1;  // Placeholder implementation
}

int CpuInfo::GetTotalCores() {
    DWORD len = 0;

    GetLogicalProcessorInformation(nullptr, &len);
    std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(len / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

    if (!GetLogicalProcessorInformation(buffer.data(), &len)) {
        return -1; // Error occurred
    }

    int coreCount = 0;
    for (const auto& info : buffer) {
        if (info.Relationship == RelationProcessorCore) {
            coreCount++;
        }
    }

    return coreCount;
}

float CpuInfo::PollGetCoreUsage(int index) {
    PDH_HQUERY hQuery;
    PDH_HCOUNTER hCounter;
    PDH_FMT_COUNTERVALUE counterVal;

    // Open a PDH query
    if (PdhOpenQuery(nullptr, 0, &hQuery) != ERROR_SUCCESS) {
        return -2; // Error in opening query
    }

    // Create the counter path for the specific core index
    std::wstring counterPath = L"\\Processor(" + std::to_wstring(index) + L")\\% Processor Time";

    // Add the counter to the query
    if (PdhAddCounter(hQuery, counterPath.c_str(), 0, &hCounter) != ERROR_SUCCESS) {
        PdhCloseQuery(hQuery);
        return -3; // Error in adding counter
    }

    // Collect initial data sample
    if (PdhCollectQueryData(hQuery) != ERROR_SUCCESS) {
        PdhCloseQuery(hQuery);
        return -4; // Error in collecting data
    }

    // Wait for the second data point (e.g., 1 second) to calculate CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Collect the second data sample
    if (PdhCollectQueryData(hQuery) != ERROR_SUCCESS) {
        PdhCloseQuery(hQuery);
        return -5; // Error in collecting second data
    }

    // Get the formatted counter value as a double
    if (PdhGetFormattedCounterValue(hCounter, PDH_FMT_DOUBLE, nullptr, &counterVal) != ERROR_SUCCESS) {
        PdhCloseQuery(hQuery);
        return -6; // Error in formatting counter value
    }

    // Close the PDH query
    PdhCloseQuery(hQuery);

    // Return the CPU usage percentage for the core
    return static_cast<float>(counterVal.doubleValue);
}