#pragma once
#include <cstdint>
#include <string>

namespace GUI 
{
	class GUIBaseClass {
	public: 
		uint16_t width;
		uint16_t height;
		std::string title;
		bool show;
		
		virtual void Draw() = 0;

	};
}