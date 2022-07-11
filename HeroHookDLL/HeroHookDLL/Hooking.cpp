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

	if(MH_CreateHook(VFunc(GUI::device,42),&EndScene,reinterpret_cast<void**>(&EndSceneOriginal)))
		throw std::runtime_error("Minhook ded, hooking didnt work");

	if (MH_CreateHook(VFunc(GUI::device, 16), &Reset, reinterpret_cast<void**>(&ResetOriginal)))
		throw std::runtime_error("Minhook ded, hooking didnt work");


	if(MH_EnableHook(MH_ALL_HOOKS))
		throw std::runtime_error("Minhook ded");

	GUI::DestroyDx();

}
void Hooking::Destroy() noexcept 
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}


long __stdcall Hooking::EndScene(IDirect3DDevice9* device) noexcept 
{
	const auto result = EndSceneOriginal(device, device);
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