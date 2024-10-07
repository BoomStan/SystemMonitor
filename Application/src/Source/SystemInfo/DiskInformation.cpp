#include "../../Headers/DiskInformation.h"

#ifdef max
#undef max
#endif

std::string DiskInformation::ConvertWStringToString(const std::wstring& wstr) {
    std::string result;
    result.reserve(wstr.size());

    for (wchar_t wc : wstr) {
        result.push_back(static_cast<char>(wc)); // Simple cast (works for ASCII-compatible wide chars)
    }

    return result;
}
// Function to gather disk information
void DiskInformation::DiskThreadFunction() {
    while (running) {
        for (size_t i = 0; i < diskNames.size(); ++i) {
            const std::wstring& diskName = diskNames[i];
            DiskData& data = diskDataList[i];

            ULARGE_INTEGER freeBytesAvailable, totalBytes, freeBytes;
            if (GetDiskFreeSpaceExW(diskName.c_str(), &freeBytesAvailable, &totalBytes, &freeBytes)) {
                data.totalSpace = totalBytes.QuadPart;
                data.freeSpace = freeBytes.QuadPart;
                data.usedSpace = totalBytes.QuadPart - freeBytes.QuadPart;

                // Ensure usedSpace is not negative
                if (data.usedSpace < 0) {
                    std::cerr << "Error: Negative used space calculated for " << ConvertWStringToString(diskName) << std::endl;
                    data.usedSpace = 0; // Set to zero to avoid display issues
                }

                // Continue with the write data calculation
                data.lastWrittenBytes = data.currentWrittenBytes;
                data.currentWrittenBytes = data.totalSpace - data.freeSpace; // Example calculation
                // More logic...
            }
            else {
                std::cerr << "Error fetching disk information for " << ConvertWStringToString(diskName) << std::endl;
                std::cerr << "GetLastError: " << GetLastError() << std::endl; // Log error code
            }
        }

        // Sleep for a while before polling again (e.g., 1 second)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// Function to start monitoring disk data
void DiskInformation::Start() {
    if (running) return; // Avoid starting multiple times

    // Clear existing disk data if any
    diskNames.clear();
    diskDataList.clear();

    // Buffer for drive letters (e.g., "C:\\", "D:\\", etc.)
    wchar_t driveBuffer[256];
    DWORD result = GetLogicalDriveStringsW(ARRAYSIZE(driveBuffer), driveBuffer);

    if (result == 0) {
        std::cerr << "Error: Unable to get logical drive strings." << std::endl;
        return;
    }

    // Iterate through the buffer and get all valid drives
    for (wchar_t* drive = driveBuffer; *drive != '\0'; drive += wcslen(drive) + 1) {
        if (GetDriveTypeW(drive) == DRIVE_FIXED) {
            // Add only fixed drives (e.g., C:\, D:\) to the diskNames list
            diskNames.push_back(drive);
        }
    }

    // Resize the data list to match the number of detected disks
    diskDataList.resize(diskNames.size());

    if (diskNames.empty()) {
        std::cerr << "No fixed disks detected." << std::endl;
        return;
    }

    running = true;
    diskThread = std::thread(&DiskInformation::DiskThreadFunction, this);
}

// Function to stop monitoring disk data
void DiskInformation::Stop() {
    running = false;
    if (diskThread.joinable()) {
        diskThread.join();
    }
}

// Function to display disk information using ImGui
void DiskInformation::Display() {
    if (diskDataList.empty()) {
        ImGui::Begin("Disk Information");
        ImGui::Text("No disk data available.");
        ImGui::End();
        return;
    }

    // Iterate over all detected disks and create a separate window for each
    for (size_t i = 0; i < diskDataList.size(); ++i) {
        const auto& data = diskDataList[i];

        // Create a unique window title for each disk
        std::string windowTitle = "Disk " + ConvertWStringToString(diskNames[i]);

        // Begin a new window for each disk
        ImGui::Begin(windowTitle.c_str());



        // Display the disk information
        ImGui::Text("Total Space: %.2f GB", static_cast<float>(data.totalSpace) / (1024 * 1024 * 1024));
        ImGui::Text("Used Space: %.2f GB", std::max(0.0f, static_cast<float>(data.usedSpace) / (1024 * 1024 * 1024)));
        ImGui::Text("Free Space: %.2f GB", std::max(0.0f, static_cast<float>(data.freeSpace) / (1024 * 1024 * 1024)));

        // Display the current write speed dynamically formatted (KB, MB, GB)
        std::string formattedWriteSpeed = FormatBytes(data.writeSpeedKBps * 1024); // Convert KBps to Bytes
        ImGui::Text("Write Speed: %s", formattedWriteSpeed.c_str());

        // End the disk-specific window
        ImGui::End();
    }
}

std::string DiskInformation::FormatBytes(long long bytes) {
    constexpr long long KB = 1024;
    constexpr long long MB = 1024 * KB;
    constexpr long long GB = 1024 * MB;

    char buffer[64];

    if (bytes >= GB) {
        snprintf(buffer, sizeof(buffer), "%.2f GB", static_cast<float>(bytes) / GB);
    }
    else if (bytes >= MB) {
        snprintf(buffer, sizeof(buffer), "%.2f MB", static_cast<float>(bytes) / MB);
    }
    else {
        snprintf(buffer, sizeof(buffer), "%.2f KB", static_cast<float>(bytes) / KB);
    }

    return std::string(buffer);
}