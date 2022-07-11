#include "Hooking.h"
#include <stdexcept>
#include <intrin.h>
#include "../include/MinHook.h"
#include "../imGUI/imgui.h"
#include "../imGUI/imgui_impl_win32.h"
#include "../imGUI/imgui_impl_dx9.h"

void Hooking::Init()
{
	if (MH_Initialize())
		throw std::runtime_error("Minhook ded");

	if (MH_CreateHook(VFunc(GUI::device, 42), &EndScene, reinterpret_cast<void**>(&EndSceneOriginal)))
		throw std::runtime_error("Minhook ded, hooking didnt work");

	if (MH_CreateHook(VFunc(GUI::device, 16), &Reset, reinterpret_cast<void**>(&ResetOriginal)))
		throw std::runtime_error("Minhook ded, hooking didnt work");

	if (MH_CreateHook(VFunc(Hooking::lpDInput, 9), &GetDeviceState, reinterpret_cast<void**>(&GetDeviceStateOriginal)))
		throw std::runtime_error("Minhook ded, hooking didnt work");

	if (MH_EnableHook(MH_ALL_HOOKS))
		throw std::runtime_error("Minhook ded");

	GUI::DestroyDx();

}
bool Hooking::InitInputHook(HMODULE hInst)
{
	if (DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&Hooking::DInput, NULL) != DI_OK) {
		throw std::runtime_error("Couldnt Create DirectInputDevice");
		return false;
	}

	if (Hooking::DInput->CreateDevice(GUID_SysMouse, &Hooking::lpDInput, NULL) != DI_OK) {
		throw std::runtime_error("Couldnt Create lpDInput");
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

	if (GUI::showMenu)
		GUI::Draw();

	return result;
}

HRESULT __stdcall Hooking::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto result = ResetOriginal(device, device, params);
	ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}

HRESULT __stdcall Hooking::GetDeviceData(IDirectInputDevice8*, DWORD d, LPDIDEVICEOBJECTDATA l, LPDWORD w, DWORD dw)
{

	return GetDeviceDataOriginal(Hooking::lpDInput, d, l, w, dw);
}
HRESULT __stdcall Hooking::GetDeviceState(IDirectInputDevice8* pThis, DWORD cbData, LPVOID lpvData)
{
	HRESULT result = GetDeviceStateOriginal(pThis, cbData, lpvData);

	if (result == DI_OK) {
		if (cbData == sizeof(DIMOUSESTATE)) {//caller is a mouse
			if (((LPDIMOUSESTATE)lpvData)->rgbButtons[0] != 0) {
				//std::cout << "[LMB]" << std::endl;
			}
			if (((LPDIMOUSESTATE)lpvData)->rgbButtons[1] != 0) {
				//std::cout << "[RMB]" << std::endl;
			}
		}
		if (cbData == sizeof(DIMOUSESTATE2)) {//caller is also a mouse but different struct
			if (((LPDIMOUSESTATE2)lpvData)->rgbButtons[0] != 0) {
				//std::cout << "[LMB2]" << std::endl;
			}
			if (((LPDIMOUSESTATE2)lpvData)->rgbButtons[1] != 0) {
				//std::cout << "[RMB2]" << std::endl;
			}
		}
		if (GUI::showMenu) {
			((LPDIMOUSESTATE2)lpvData)->rgbButtons[0] = 0;
			((LPDIMOUSESTATE2)lpvData)->rgbButtons[1] = 0;
			((LPDIMOUSESTATE2)lpvData)->lZ = 0;

			//Why I'm using this instead of the lpvData
			//The lpvData was shit and didnt work well, this works 100% better
			if (GetAsyncKeyState(VK_LBUTTON))
				ImGui::GetIO().AddMouseButtonEvent(0, true);
			else
				ImGui::GetIO().AddMouseButtonEvent(0, false);

			if (GetAsyncKeyState(VK_RBUTTON))
				ImGui::GetIO().AddMouseButtonEvent(1, true);
			else
				ImGui::GetIO().AddMouseButtonEvent(1, false);

		}
	}


	return result;
}