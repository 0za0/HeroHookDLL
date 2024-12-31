#pragma once
#include <d3d9.h>
#include "../imGUI/imgui.h"


namespace GUI
{

	//GUI Stuff
	inline ImFont* font;

	inline bool showConsole = false;
	inline bool showDebugEnabler = false;


	inline bool showMenu = true;

	inline bool showConsole = false;
	inline bool showDebugEnabler = false;

	inline bool isInitialized = false;
	inline bool showCoordinates = false;

	inline HWND window = nullptr;
	inline WNDCLASSEX windowClass = {  };
	inline WNDPROC originalWindowProc = nullptr;

	//DirectX Stuff
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline LPDIRECT3D9 d3d9 = nullptr;

	bool InitializeWindowClass(const char* windowClassName) noexcept;
	void DestroyWindowClass() noexcept;
	
	bool InitializeWindow(const char* windowName) noexcept;
	void DestroyWindow() noexcept;

	//DirectX Stuff
	bool InitDx() noexcept;
	void DestroyDx() noexcept; 
	void LogToConsole(const char* input) noexcept;
	void Initialize();

	void InitMenu(LPDIRECT3DDEVICE9 device) noexcept;
	void Destroy() noexcept;

	//Draws/Renders the menu
	void Draw() noexcept;
}