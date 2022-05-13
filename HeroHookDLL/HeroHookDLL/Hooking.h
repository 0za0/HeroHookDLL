#pragma once
#ifndef HOOKING_H
#define HOOKING_H

#include <d3d9.h>
#include <d3dx9.h>
#include <MinHook.h>

extern LPD3DXFONT font;

extern bool coordinateToggleOn;
//EndScene Hook

typedef HRESULT(__stdcall* endScene)(IDirect3DDevice9* pDevice);
typedef HRESULT(__stdcall* beginScene)(IDirect3DDevice9* pDevice);


extern endScene pEndScene; //Original Pointer


//DetourFunction
extern HRESULT __stdcall DetourEndScene(IDirect3DDevice9* pDevice);
extern HRESULT __stdcall DetourBeginScene(IDirect3DDevice9* pDevice);


//MinHook Init
void InitMinHook();
#endif
