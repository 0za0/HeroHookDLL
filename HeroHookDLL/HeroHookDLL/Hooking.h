#pragma once
#include "GUI.h"
#include <dinput.h>

#define DIRECTINPUT_VERSION 0x0800
#pragma comment (lib,"Dinput8.lib")
#pragma comment (lib,"Dxguid.lib")

namespace Hooking
{
	inline IDirectInput8* DInput = nullptr;
	inline LPDIRECTINPUTDEVICE8A lpDInput = NULL;

	using GetDeviceDataT = HRESULT(__stdcall*)(IDirectInputDevice8*, DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
	using  GetDeviceStateT = HRESULT(__stdcall*)(IDirectInputDevice8* pThis, DWORD cbData, LPVOID lpvData);
	HRESULT __stdcall GetDeviceData (IDirectInputDevice8*, DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
	HRESULT __stdcall GetDeviceState (IDirectInputDevice8* pThis, DWORD cbData, LPVOID lpvData);


	inline GetDeviceStateT GetDeviceStateOriginal = nullptr; //pointer to original function
	inline GetDeviceDataT GetDeviceDataOriginal = nullptr; //pointer to original function

	bool InitInputHook(HINSTANCE hInst);

	void Init();
	void Destroy() noexcept;

	constexpr void* VFunc(void* thisptr, size_t index) noexcept
	{
		return (*static_cast<void***>(thisptr))[index];
	}

	//newer way of typedefs apparently
	using EndSceneFunc = long(__thiscall*)(void*, IDirect3DDevice9*) noexcept;
	inline EndSceneFunc EndSceneOriginal = nullptr;
	long __stdcall EndScene(IDirect3DDevice9* device) noexcept;

	using ResetFunc = HRESULT(__thiscall*)(void*, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*) noexcept;
	inline ResetFunc ResetOriginal = nullptr;
	HRESULT __stdcall Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept;




}