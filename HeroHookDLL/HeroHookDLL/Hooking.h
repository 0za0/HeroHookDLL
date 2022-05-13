#pragma once
#ifndef HOOKING_H
#define HOOKING_H

#include <d3d9.h>
#include <d3dx9.h>
#include <MinHook.h>

extern LPD3DXFONT font;
//EndScene Hook



//DetourFunction
extern HRESULT __stdcall DetourEndScene(IDirect3DDevice9* pDevice);
extern HRESULT __stdcall DetourBeginScene(IDirect3DDevice9* pDevice);


//MinHook Init
void InitMinHook();
#endif
