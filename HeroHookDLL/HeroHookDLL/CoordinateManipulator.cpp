#include "CoordinateManipulator.hpp"
#include "../imGUI/imgui_stdlib.h"
#include "FileUtilities.hpp"


namespace GUI {

	// Instead of a global PositionCode, keep a local buffer for the title
	static char titleBuffer[256] = "";

	// This array holds the current coordinates to display in the "Set Coordinates" slider
	static float coordsToSet[3] = { 0.f, 0.f, 0.f };

	void GUI::CoordinateManipulator::Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(100, 200), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin(title.c_str(), &(this->show))) {
			ImGui::End();
			return;
		}

		if (coordinates && coordinates->isValid) {
			// Layout
			ImVec2 availableSpace = ImGui::GetContentRegionAvail();
			float minWidth = 200.0f;
			float splitRatio = 0.5f;
			float leftWidth = std::max(availableSpace.x * splitRatio, minWidth);
			float rightWidth = std::max(availableSpace.x * (1.0f - splitRatio), minWidth);

			// Left-side panel
			ImGui::BeginChild("Left Controls", ImVec2(leftWidth, 0), true);
			{
				// Current coordinates
				ImGui::Text("Coordinates:");
				ImGui::Text("X: %.2f | Y: %.2f | Z: %.2f",
					coordinates->x, coordinates->y, coordinates->z);

				ImGui::SeparatorText("Manipulation");

				// Slide floats
				ImGui::SliderFloat3("Set Coordinates", coordsToSet, -999.9f, 999.9f, "%.2f");

				// Copy coords into the slider
				if (ImGui::Button("COPY COORDS")) {
					memcpy(coordsToSet, coordinates, sizeof(float) * 3);
				}

				// Apply coordsToSet to the system
				if (ImGui::Button("SET COORDS")) {
					addToCoordinates(coordsToSet[0], coordsToSet[1], coordsToSet[2]);
				}

				ImGui::SeparatorText("Position Code Manager");

				// Title input as a char buffer
				ImGui::InputText("Position Code Name", titleBuffer, IM_ARRAYSIZE(titleBuffer));

				ImGui::Text("COORDS TO STORE: X: %.2f | Y: %.2f | Z: %.2f",
					coordsToSet[0], coordsToSet[1], coordsToSet[2]);

				// Store current coords in coordsToSet
				if (ImGui::Button("USE CURRENT COORDS")) {
					memcpy(coordsToSet, coordinates, sizeof(float) * 3);
				}

				if (ImGui::Button("Save Position Code to List"))
				{
					PositionCode newPositionCode;
					newPositionCode.x = coordsToSet[0];
					newPositionCode.y = coordsToSet[1];
					newPositionCode.z = coordsToSet[2];
					newPositionCode.levelId = getLevelId();
					strncpy_s(newPositionCode.title, titleBuffer, sizeof(newPositionCode.title));

					positionCodes.push_back(newPositionCode);

					titleBuffer[0] = '\0';

					// Write the entire list to file
					writePositionCodesToFile(&positionCodes);
					positionCodes = readPositionCodesFromFile();
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();

			// Right-side list
			ImGui::BeginChild("Position Code List", ImVec2(rightWidth, 0), true);
			{
				for (int index = 0; index < positionCodes.size(); ++index) {
					const auto& position = positionCodes[index];

					if (position.levelId == getLevelId()) {
						if (ImGui::Selectable(position.title)) {
							addToCoordinates(position.x, position.y, position.z);
						}
					}
				}
			}
			ImGui::EndChild();
		}
		else {
			ImGui::Text("Coordinates: Invalid");
		}

		ImGui::End();
	}
}