#pragma once
#include "GUI.h"

namespace Hooking
{
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