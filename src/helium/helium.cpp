#include <string>
#include <iostream>
#include "ImGuiFileDialog.h"

#include "../injectDLL/injectDLL.h"

#include "helium.h"

namespace Helium {
    // PID input buffer
    char pidBuffer[10] = "";
    // DLL path buffer
    char pathBuffer[512] = "";
    // Injector status
    char status[256] = "Waiting...";

	void RenderUI() {
        // Create window
        ImGui::Begin("HeliumDLLInjector", NULL, ImGuiWindowFlags_NoBringToFrontOnFocus);
        // Quit button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.93f, 0.32f, 0.31f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 0, 0, 1));
        if (ImGui::Button("Quit"))
            exit(0);
        ImGui::PopStyleColor(1);
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        // Inject button
        if (ImGui::Button("Inject")) {
            std::cout << "[DEBUG] Ready to inject DLL(" << pathBuffer << ") to PID(" << pidBuffer << ")" << std::endl;
            DLLInjector::Inject(pidBuffer, pathBuffer, status);
        }
        // PID input
        ImGui::InputText("Target PID", pidBuffer, sizeof(pidBuffer));
        // DLL input
        ImGui::InputText("DLL File", pathBuffer, sizeof(pathBuffer));
        ImGui::SameLine();
        // DLL browse
        if (ImGui::Button("Browse")) {
            std::cout << "[DEBUG] Choosing DLL" << std::endl;
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseDllFile", "Choose DLL File", ".dll", config);
        }
        if (ImGuiFileDialog::Instance()->Display("ChooseDllFile")) {
            if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                // action
                strcpy_s(pathBuffer, filePathName.c_str());
                std::cout << "[DEBUG] pathBuffer: " << pathBuffer << std::endl;
            }
            // close
            ImGuiFileDialog::Instance()->Close();
        }
        // Status display
        ImGui::Text(status);
        ImGui::End();
	}
}