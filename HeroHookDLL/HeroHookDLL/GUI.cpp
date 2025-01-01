#include <stdexcept>

#include "GUI.h"

#include "../imGUI/imgui.h"
#include "../imGUI/imgui_impl_win32.h"
#include "../imGUI/imgui_impl_dx9.h"
#include "roboto.cpp"
#include "GameManip.h"
//TODO: Make all GUI windows a class.
#include "Console.cpp"
#include "DebugEnabler.cpp"
#include "CoordinateWindow.cpp"
#include "CoordinateManipulator.hpp"
#include "FileUtilities.hpp"

static Console console;
static DebugEnabler debugger;
Coordinates sharedCoordinates;
GUI::CoordinateManipulator manipulator;


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//wndproc
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool GUI::InitializeWindowClass(const char* windowClassName) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszClassName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = NULL;

	if (!RegisterClassEx(&windowClass))
		return false;

	return true;
}
void GUI::DestroyWindowClass() noexcept
{
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool GUI::InitializeWindow(const char* windowName) noexcept
{
	//Create Temp Window to get DX9 Device lol
	window = CreateWindow(windowClass.lpszClassName, windowName, WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, 0, 0, windowClass.hInstance, 0);
	if (!window)
		return false;
	return true;
}
void GUI::DestroyWindow() noexcept
{
	if (window)
		DestroyWindow(window);
}

//DirectX Stuff
bool GUI::InitDx() noexcept
{
	const auto handle = GetModuleHandle("d3d9.dll");
	if (!handle)
		return false;

	//pretty much a typedef
	using CreateFunction = LPDIRECT3D9(__stdcall*)(UINT);
	const auto create = reinterpret_cast<CreateFunction>(GetProcAddress(handle, "Direct3DCreate9"));

	if (!create)
		return false;
	d3d9 = create(D3D_SDK_VERSION);

	if (!d3d9)
		return false;

	D3DPRESENT_PARAMETERS params = {};
	params.BackBufferWidth = 0;
	params.BackBufferHeight = 0;
	params.BackBufferFormat = D3DFMT_UNKNOWN;
	params.BackBufferCount = 0;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality = NULL;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = window;
	params.Windowed = 1;
	params.EnableAutoDepthStencil = 0;
	params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	params.Flags = NULL;
	params.FullScreen_RefreshRateInHz = 0;
	params.PresentationInterval = 0;

	if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &params, &device) < 0)
		return false;

	return true;
}

void GUI::DestroyDx() noexcept
{
	if (device)
	{
		device->Release();
		device = NULL;
	}

	if (d3d9)
	{
		d3d9->Release();
		d3d9 = NULL;
	}
}

void GUI::Initialize()
{
	if (!InitializeWindowClass("Bazoinkle"))
		throw std::runtime_error("Window Class Init Failed.");

	if (!InitializeWindow("Bazoinkle Heroes"))
		throw std::runtime_error("Window Init Failed");

	if (!InitDx())
		throw std::runtime_error("DirectX Init Failed");

	DestroyWindow();
	DestroyWindowClass();
}


void GUI::InitMenu(LPDIRECT3DDEVICE9 device) noexcept
{
	//Initialization
	manipulator = CoordinateManipulator(400, 200, "Coordinate Manipulator", false, &sharedCoordinates);
	manipulator.SetPositionCodes(readPositionCodesFromFile());


	auto params = D3DDEVICE_CREATION_PARAMETERS{  };
	device->GetCreationParameters(&params);

	window = params.hFocusWindow;

	originalWindowProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc)));

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

	isInitialized = true;

	ImGuiIO& io = ImGui::GetIO();
	GUI::font = io.Fonts->AddFontFromMemoryCompressedTTF(roboto_compressed_data, roboto_compressed_size, 30);
}

void GUI::Destroy() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalWindowProc));

	DestroyDx();
}
void GUI::LogToConsole(const char* input) noexcept {
	console.AddLog(input);
}

void GUI::Draw() noexcept
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	io.MouseDrawCursor = true;

	ImGui::PushFont(GUI::font);
	ImGui::BeginMainMenuBar();  // This begins the menu bar, and it should only have one corresponding End call.

	if (ImGui::BeginMenu("Windows"))
	{
		if (ImGui::MenuItem("Console"))
			GUI::showConsole = !GUI::showConsole;

		if (ImGui::MenuItem("Debug Enabler"))
			GUI::showDebugEnabler = !GUI::showDebugEnabler;
		if (ImGui::MenuItem("Coordinate Display"))
			manipulator.show = !manipulator.show;

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();  // Correct placement of EndMainMenuBar

	// Drawing individual windows
	if (GUI::showConsole)
		console.Draw("HeroHookDLL Console", &GUI::showConsole);

	if (GUI::showDebugEnabler)
		debugger.Draw("Debug Enabler", &GUI::showDebugEnabler);

	if (manipulator.show)
	{
		getCoordinates(sharedCoordinates);
		manipulator.Draw();
	}

	ImGui::PopFont();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (GetAsyncKeyState(VK_F12) & 1)
		GUI::showMenu = !GUI::showMenu;
	//Pass to imGUI
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam) && GUI::showMenu)
	{
		return 1L;
	}
	return CallWindowProc(GUI::originalWindowProc, hWnd, msg, wParam, lParam);
}