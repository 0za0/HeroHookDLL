#define NOMINMAX
#pragma once
#include "GUIWindowBase.hpp"
#include "GameManip.h"
#include "../imGUI/imgui.h"
#include <vector>
#include <algorithm>


namespace GUI {

	class CoordinateManipulator : public GUIBaseClass {
		Coordinates* coordinates;
		float coordsToSet[4] = { 0.0 ,0.0 ,0.0, 0.0 };
		ImVector<PositionCode> positionCodes;
	public:
		CoordinateManipulator() :GUIBaseClass() {
			width = 0;
			height = 0;
			title = "";
			show = false;
			coordinates = nullptr;
		};

		CoordinateManipulator(uint16_t w, uint16_t h, const std::string& t, bool s, Coordinates* coords) : GUIBaseClass() {
			width = w;
			height = h;
			title = t;
			show = s;
			coordinates = coords;
		}
		void SetPositionCodes(ImVector<PositionCode> posCodes) {
			positionCodes = posCodes;
		}

		void Draw() override;
	};
}