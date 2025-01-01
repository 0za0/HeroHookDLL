#include "CoordinateManipulator.hpp"
#include "../imGUI/imgui_stdlib.h"
#include "FileUtilities.hpp"


namespace GUI {
	PositionCode positionCode;
    void GUI::CoordinateManipulator::Draw() {
        ImGui::SetNextWindowSize(ImVec2(100, 200), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin(title.c_str(), &(this->show))) {
            ImGui::End();
            return;
        }

        if (coordinates && coordinates->isValid) {
            // Get available space in the parent window
            ImVec2 availableSpace = ImGui::GetContentRegionAvail();
            float minWidth = 200.0f; 
            float splitRatio = 0.5f; 
            float leftWidth = std::max(availableSpace.x * splitRatio, minWidth);
            float rightWidth = std::max(availableSpace.x * (1.0f - splitRatio), minWidth);

            // Left-side controls
            ImGui::BeginChild("Left Controls", ImVec2(leftWidth, 0), true);
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
                    memcpy(&positionCode, coordinates, sizeof(float) * 3);
                }

                if (ImGui::Button("Save Position Code to List")) {
                    PositionCode newPositionCode = positionCode;
                    newPositionCode.levelId = getLevelId();
                    positionCodes.push_back(newPositionCode);
                    writePositionCodesToFile(&positionCodes);
                }
            }
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::BeginChild("Position Code List", ImVec2(rightWidth, 0), true);
            {
                for (size_t index = 0; index < positionCodes.size(); ++index) {
                    const auto& position = positionCodes[index];

                    if (position.levelId == getLevelId()) {
                        if (ImGui::Selectable(position.title.c_str())) {
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