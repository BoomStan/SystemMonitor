#include "../../Headers/NetworkInfo.h"

#include <PdhMsg.h>    // For PDH error messages (optional but useful)
#include <vector>
#include <unordered_set>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h> // IP Helper API
#pragma comment(lib, "Ws2_32.lib") // Link against Ws2_32.lib
#pragma comment(lib, "iphlpapi.lib") // Link with Iphlpapi.lib
#include <netioapi.h> 
#include <codecvt> // Include for string conversion
#include <locale>  // Include for locale support
#define _WIN32_WINNT 0x0600


// Global Variables
std::vector<std::wstring> interfaceNames; // To store available interface names
int selectedInterfaceIndex = -1;           // To track selected interface
bool running = false;                       // Flag for running state
long long lastBytesSent = 0;                // Last sent bytes counter
long long lastBytesRecv = 0;                // Last received bytes counter
float uploadSpeed = 0.0f;                   // Upload speed in bytes
float downloadSpeed = 0.0f;                 // Download speed in bytes

void NetworkInfo::Start() {
    running = true; // Set running flag
    lastBytesSent = 0;
    lastBytesRecv = 0;
    // Start the network thread
    networkThread = std::thread(&NetworkInfo::NetworkThreadFunction, this);
}

void NetworkInfo::Display() {
    ImGui::Begin("Network Info##Window");

    // Check if interfaceNames is empty
    if (interfaceNames.empty()) {
        ImGui::Text("No network interfaces available.");
        ImGui::End();
        return; // Exit early if no interfaces
    }

    // Validate the selectedInterfaceIndex
    if (selectedInterfaceIndex < 0 || selectedInterfaceIndex >= static_cast<int>(interfaceNames.size())) {
        std::cerr << "Error: selectedInterfaceIndex (" << selectedInterfaceIndex << ") is out of bounds!" << std::endl;
        selectedInterfaceIndex = 0; // Reset to first valid index
    }

    // Prepare combo label
    // Store the converted string in a local variable so it persists during rendering
    std::string comboLabel = (selectedInterfaceIndex >= 0)
        ? NetworkInfo::ConvertWStringToString(interfaceNames[selectedInterfaceIndex])
        : "None";

    // Start the combo for interface selection
    if (ImGui::BeginCombo("Select Interface", comboLabel.c_str())) {
        for (size_t i = 0; i < interfaceNames.size(); ++i) {
            std::string interfaceNameStr = NetworkInfo::ConvertWStringToString(interfaceNames[i]);
            bool isSelected = (selectedInterfaceIndex == static_cast<int>(i));

            // Append the index `##i` to make each item in the combo unique
            if (ImGui::Selectable((interfaceNameStr + "##" + std::to_string(i)).c_str(), isSelected)) {
                selectedInterfaceIndex = static_cast<int>(i); // Set the selected index
            }

            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Display upload and download speeds
    if (selectedInterfaceIndex >= 0) {
        ImGui::Text("Upload Speed: %.2f KB/s", uploadSpeed / 1024.0f);
        ImGui::Text("Download Speed: %.2f KB/s", downloadSpeed / 1024.0f);
    }
    else {
        ImGui::Text("Please select an interface.");
    }

    ImGui::End();
}

// Utility function to convert std::wstring to std::string
std::string NetworkInfo::ConvertWStringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

void NetworkInfo::Stop() {
    running = false; // Set running flag to false
    if (networkThread.joinable()) {
        networkThread.join(); // Wait for the thread to finish
    }
}

void NetworkInfo::UpdateNetworkData() {
    MIB_IF_ROW2 ifRow;
    DWORD dwRetVal = 0;

    // Initialize the interface structure
    ZeroMemory(&ifRow, sizeof(MIB_IF_ROW2));

    // Declare a pointer for the interface table
    PMIB_IF_TABLE2 pIfTable = nullptr;

    // Query all interfaces using GetIfTable2
    dwRetVal = GetIfTable2(&pIfTable);
    if (dwRetVal != NO_ERROR) {
        std::wcout << L"GetIfTable2 failed with error: " << dwRetVal << std::endl;
        return;
    }

    // Create a set to track processed interface descriptions
    std::unordered_set<std::wstring> processedInterfaces;

    // Clear previous interface names
    interfaceNames.clear();

    // Loop through the interfaces
    for (int i = 0; i < static_cast<int>(pIfTable->NumEntries); i++) {
        ifRow = pIfTable->Table[i]; // Get each row

        // Skip irrelevant interfaces
        if (wcsstr(ifRow.Description, L"Virtual") ||
            wcsstr(ifRow.Description, L"WAN Miniport") ||
            wcsstr(ifRow.Description, L"Hyper-V")) {
            continue; // Skip to the next iteration
        }

        // Only process operational interfaces
        if (ifRow.OperStatus == IfOperStatusUp) {
            // Check for duplicates
            if (processedInterfaces.find(ifRow.Description) != processedInterfaces.end()) {
                continue; // Skip this duplicate interface
            }

            // Add this interface description to the set of processed interfaces
            processedInterfaces.insert(ifRow.Description);
            interfaceNames.push_back(ifRow.Description); // Store the interface name

            // If this is the selected interface, calculate speeds
            if (selectedInterfaceIndex == static_cast<int>(interfaceNames.size()) - 1) {
                // Fetch 64-bit byte counters
                long long currentBytesSent = ifRow.OutOctets;
                long long currentBytesRecv = ifRow.InOctets;

                // Calculate the number of bytes sent and received since last check
                long long bytesSentDiff = currentBytesSent - lastBytesSent;
                long long bytesRecvDiff = currentBytesRecv - lastBytesRecv;

                // Update the previous byte counts for the next interval
                lastBytesSent = currentBytesSent;
                lastBytesRecv = currentBytesRecv;

                // Only output when there is a significant change
                if (bytesSentDiff > 0 || bytesRecvDiff > 0) {
                    // Calculate upload/download speeds based on the time interval
                    uploadSpeed = bytesSentDiff; // Bytes sent in the last interval
                    downloadSpeed = bytesRecvDiff; // Bytes received in the last interval
                }
            }
        }
    }

    // Free allocated memory
    FreeMibTable(pIfTable);
}

void NetworkInfo::NetworkThreadFunction() {
    while (running) {
        UpdateNetworkData(); // Update the network data
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Update every 100ms
    }
}