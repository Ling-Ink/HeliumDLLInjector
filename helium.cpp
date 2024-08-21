#include <string>

#include "helium.h"

namespace Helium {
    // PID input buffer
    char pidBuffer[10] = "";
    // DLL path buffer
    char pathBuffer[256] = "";

	void RenderUI() {
        ImGui::Begin("HeliumDLLInjector");

        if (ImGui::Button("Quit"))
            exit(0);
        ImGui::SameLine();
        if (ImGui::Button("Inject")) {
            //单击事件处理程序
        }
        ImGui::InputText("Target PID", pidBuffer, sizeof(pidBuffer));
        // Coding transfering
        std::string pidU8 = pidBuffer;

        ImGui::InputText("DLL File", pathBuffer, sizeof(pathBuffer));
        // Coding transfering
        std::string pathU8 = pathBuffer;
        ImGui::SameLine();
        if (ImGui::Button("Browse")) {
            //单击事件处理程序
        }
        ImGui::End();
	}
}