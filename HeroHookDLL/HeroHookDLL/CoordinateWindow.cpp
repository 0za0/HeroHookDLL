#include "../imGUI/imgui.h"
#include <vector>
#include <string>
#include "GameManip.h"


struct CoordinateDisplay {

	void Draw(const std::string title, bool* p_open, std::string coords) {
		ImGui::SetNextWindowSize(ImVec2(100, 200), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin(title.c_str(), p_open))
		{
			ImGui::End();
			return;
		}

		ImGui::Text(coords.c_str());
		ImGui::End();
	}
};