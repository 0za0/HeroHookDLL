#include "CoordinateManipulator.hpp"
namespace GUI {
	void CoordinateManipulator::Draw() {
		ImGui::SetNextWindowSize(ImVec2(100, 200), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin(title.c_str(), &(this->show)))
		{
			ImGui::End();
			return;
		}
		if (coordinates && coordinates->isValid) {
			ImGui::Text("Coordinates:");
			ImGui::Text("X: %.2f", coordinates->x);
			ImGui::Text("Y: %.2f", coordinates->y);
			ImGui::Text("Z: %.2f", coordinates->z);
			ImGui::Separator();
			ImGui::SliderFloat3("Set Coordinates", coordsToSet, -999.9, 999.9, "%.2f");

			if (ImGui::Button("COPY COORDS")) {
				memcpy(coordsToSet, coordinates, sizeof(float) * 3);
			}

			if (ImGui::Button("SET COORDS")) {
				addToCoordinates(coordsToSet[0], coordsToSet[1], coordsToSet[2]);
			}

		}
		else {
			ImGui::Text("Coordinates: Invalid");
		}
		ImGui::End();
	}
}