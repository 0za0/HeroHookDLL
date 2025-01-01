#include "CoordinateManipulator.hpp"
#include "../imGUI/imgui_stdlib.h"
namespace GUI {
	PositionCode positionCode;
	void CoordinateManipulator::Draw() {
		ImGui::SetNextWindowSize(ImVec2(100, 200), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin(title.c_str(), &(this->show)))
		{
			ImGui::End();
			return;
		}
		if (coordinates && coordinates->isValid) {
			ImGui::BeginGroup(); // Group the main controls
			{
				ImGui::Text("Coordinates:");
				ImGui::Text("X: %.2f\tZ: %.2f\tY: %.2f", coordinates->x, coordinates->y, coordinates->z);
				ImGui::SeparatorText("Manipulation");
				ImGui::SliderFloat3("Set Coordinates", coordsToSet, -999.9, 999.9, "%.2f");
				if (ImGui::Button("COPY COORDS")) {
					memcpy(coordsToSet, coordinates, sizeof(float) * 3);
				}

				if (ImGui::Button("SET COORDS")) {
					addToCoordinates(coordsToSet[0], coordsToSet[1], coordsToSet[2]);
				}

				ImGui::SeparatorText("Position Code Manager");
				ImGui::InputText("Position Code Name", &positionCode.title);
				ImGui::Text("X: %.2f\tZ: %.2f\tY: %.2f", coordinates->x, coordinates->y, coordinates->z);

				if (ImGui::Button("USE CURRENT COORDS")) {
					// Copy current coordinates into positionCode
					memcpy(&positionCode, coordinates, sizeof(float) * 3);
				}

				if (ImGui::Button("Save Position Code to List")) {
					PositionCode newPositionCode = positionCode; // Create a copy of the current positionCode
					positionCode.levelId = getLevelId();
					positionCodes.push_back(newPositionCode);
					
				}
			}
			ImGui::EndGroup(); // End the left-side controls

			// Ensure the list appears to the right
			ImGui::SameLine();

			// Right-side list of position codes
			if (ImGui::BeginChild("Position Code List", ImVec2(200, 0), true)) {
				for (size_t index = 0; index < positionCodes.size(); ++index) {
					const auto& position = positionCodes[index];

					if (position.levelId == getLevelId()) {
						// Render a selectable item and check if it's clicked
						if (ImGui::Selectable(position.title.c_str())) {
							addToCoordinates(position.x, position.y, position.z);
						}
					}
				}
				ImGui::EndChild(); // End the right-side list
			}

		}
		else {
			ImGui::Text("Coordinates: Invalid");
		}
		ImGui::End();
	}
}