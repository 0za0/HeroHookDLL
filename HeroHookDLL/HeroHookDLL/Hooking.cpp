#include <winsock2.h>
#include <ws2tcpip.h>
#include "Hooking.h"
#include "Multiplayer.h"
#include <stdexcept>
#include <intrin.h>
#include "../include/MinHook.h"
#include "../imGUI/imgui.h"
#include "../imGUI/imgui_impl_win32.h"
#include "../imGUI/imgui_impl_dx9.h"

/***/
void Hooking::Init()
{
	if (MH_Initialize())
		throw std::runtime_error("Minhook Initialization failed.");

	//if (MH_CreateHook(VFunc(GUI::device, 42), &EndScene, reinterpret_cast<void**>(&EndSceneOriginal)))
		//throw std::runtime_error("Minhook Initialization failed. Unable to hook EndScene()");

	if (MH_CreateHook(VFunc(GUI::device, 16), &Reset, reinterpret_cast<void**>(&ResetOriginal)))
		throw std::runtime_error("Minhook Initialization failed unable to hook Reset()");

	if (MH_CreateHook(VFunc(Hooking::lpDInput, 10), &Hooking::GetDeviceData, reinterpret_cast<void**>(&GetDeviceDataOriginal)))
		throw std::runtime_error("Minhook Initialization failed. Unable to hook GetDeviceData().");


	if (MH_CreateHook(VFunc(Hooking::lpDInput, 9), &GetDeviceState, reinterpret_cast<void**>(&GetDeviceStateOriginal)))
		throw std::runtime_error("Minhook Initialization failed. Unable to hook GetDeviceState().");

	if (MH_CreateHook(VFunc(GUI::device, 17), &HookedPresent, reinterpret_cast<void**>(&PresentOriginal))) {
		throw std::runtime_error("Minhook Initialization failed. Unable to hook Present().");
	}
	

	if (MH_EnableHook(MH_ALL_HOOKS))
		throw std::runtime_error("Minhook Initialization failed. Unable to enable all hooks.");

	GUI::DestroyDx();

}
bool Hooking::InitInputHook(HMODULE hInst)
{
	if (DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&Hooking::DInput, NULL) != DI_OK) {
		throw std::runtime_error("Unable to create DirectInputDevice");
		return false;
	}

	if (Hooking::DInput->CreateDevice(GUID_SysMouse, &lpDInput, NULL) != DI_OK) {
		throw std::runtime_error("Unable to create lpDInput");
		return false;
	}

}

void Hooking::Destroy() noexcept
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}


long __stdcall Hooking::EndScene(IDirect3DDevice9* device) noexcept
{
	static const auto returnAddress = _ReturnAddress();

	const auto result = EndSceneOriginal(device, device);

	if (_ReturnAddress() == returnAddress)
		return result;

	if (!GUI::isInitialized)
		GUI::InitMenu(device);

	if (GUI::showMenu && GUI::isInitialized)
		GUI::Draw();

	return result;
}

HRESULT __stdcall Hooking::HookedPresent(IDirect3DDevice9* device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) {
	if (!GUI::isInitialized) {
		GUI::InitMenu(device);
	}

	// Update multiplayer: read position, send/recv network, draw remote cube
	// This must happen BEFORE ImGui so the cube is behind the overlay
	Multiplayer::Update(device);

	if (GUI::showMenu && GUI::isInitialized) {
		GUI::Draw();
	}

	// Call the original Present function
	return PresentOriginal(device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT __stdcall Hooking::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto result = ResetOriginal(device, device, params);
	ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}


HRESULT __stdcall Hooking::GetDeviceData(IDirectInputDevice8* pThis, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	MessageBeep(MB_ICONERROR);
	MessageBox(0, NULL, "DLL Hook Error", MB_OK | MB_ICONEXCLAMATION);
	HRESULT result = GetDeviceDataOriginal(pThis, cbObjectData, rgdod, pdwInOut, dwFlags);
	if (result == DI_OK && GUI::isInitialized) {

		


	}
	if (GUI::showMenu) {
		*pdwInOut = 0; //set array size 0
	}

	return result;
}
// Track cursor position by accumulating DirectInput deltas.
// GetCursorPos doesn't work reliably because the game calls SetCursorPos
// to center the cursor each frame (standard FPS mouse-look technique).
static float s_cursorX = -1.0f, s_cursorY = -1.0f;
static bool  s_cursorSeeded = false;

HRESULT __stdcall Hooking::GetDeviceState(IDirectInputDevice8* pThis, DWORD cbData, LPVOID lpvData)
{
	HRESULT result = GetDeviceStateOriginal(pThis, cbData, lpvData);

	if (result == DI_OK && cbData >= sizeof(DIMOUSESTATE) && GUI::isInitialized) {
		DIMOUSESTATE* ms = (DIMOUSESTATE*)lpvData;

		if (GUI::showMenu) {
			// Seed cursor at window center when menu first opens
			if (!s_cursorSeeded && GUI::window) {
				RECT rc;
				GetClientRect(GUI::window, &rc);
				s_cursorX = (float)(rc.right) / 2.0f;
				s_cursorY = (float)(rc.bottom) / 2.0f;
				s_cursorSeeded = true;
			}

			// Accumulate DirectInput deltas into our tracked cursor position
			s_cursorX += (float)ms->lX;
			s_cursorY += (float)ms->lY;

			// Clamp to window client area
			if (GUI::window) {
				RECT rc;
				GetClientRect(GUI::window, &rc);
				if (s_cursorX < 0.0f) s_cursorX = 0.0f;
				if (s_cursorY < 0.0f) s_cursorY = 0.0f;
				if (s_cursorX > (float)rc.right) s_cursorX = (float)rc.right;
				if (s_cursorY > (float)rc.bottom) s_cursorY = (float)rc.bottom;
			}

			// Feed scroll wheel before zeroing
			if (ms->lZ != 0)
				ImGui::GetIO().AddMouseWheelEvent(0.0f, (float)ms->lZ / 120.0f);

			// Suppress ALL game mouse input: axes, buttons, scroll
			memset(lpvData, 0, cbData);

			// Feed our tracked cursor position to ImGui
			ImGui::GetIO().AddMousePosEvent(s_cursorX, s_cursorY);

			// Feed mouse buttons via Win32 API
			ImGui::GetIO().AddMouseButtonEvent(0, (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0);
			ImGui::GetIO().AddMouseButtonEvent(1, (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0);
			ImGui::GetIO().AddMouseButtonEvent(2, (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0);
		} else {
			// Menu hidden — reset cursor seed so it re-centers when menu reopens
			s_cursorSeeded = false;
		}
	}

	return result;
}