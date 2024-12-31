#pragma once
#include "GUIWindowBase.hpp"
#include "GameManip.h"
#include "../imGUI/imgui.h"

namespace GUI {

	class CoordinateManipulator : public GUIBaseClass {
		Coordinates* coordinates;
		float coordsToSet[4] = { 0.0 ,0.0 ,0.0, 0.0 };
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

		void Draw() override;
	};
}