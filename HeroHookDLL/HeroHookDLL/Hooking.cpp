#include "Hooking.h"
#include "GameManip.h"
#include <stdexcept>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

//endScene pEndScene; //Original Pointer
//beginScene pBeginScene; //Original Pointer
LPD3DXFONT font;
endScene pEndScene = nullptr; //Original Pointer
//BeginScene Hook
beginScene pBeginScene; //Original Pointer



void InitMinHook()
{
	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!pD3D)
		return;
	D3DPRESENT_PARAMETERS d3dparams = { 0 };
	d3dparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dparams.hDeviceWindow = GetForegroundWindow();
	d3dparams.Windowed = true;
	IDirect3DDevice9* pDevice = nullptr;
	pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dparams.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dparams, &pDevice);

	if (!pDevice) {
		pD3D->Release();
		return;
	}

	void** vTable = *reinterpret_cast<void***>(pDevice);
	//originalEndScenePointer = vTable[42];
	if (MH_Initialize())
		throw std::runtime_error("Couldn't initialize MinHook");

	/*if (MH_CreateHookApiEx(L"d3d9.dll", "EndScene", &DetourEndScene, reinterpret_cast<void**>(&pEndScene), reinterpret_cast<void**>(&originalEndScenePointer)) != MH_OK)
		throw std::runtime_error("Bruh");*/
	if (MH_CreateHook(vTable[42],
		&DetourEndScene,
		reinterpret_cast<void**>(&pEndScene)) != MH_OK)
		throw std::runtime_error("Bruh");

	if (MH_CreateHook(vTable[41],
		&DetourBeginScene,
		reinterpret_cast<void**>(&pBeginScene)) != MH_OK)
		throw std::runtime_error("Bruh");

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		throw std::runtime_error("Bruh");

}



HRESULT __stdcall DetourBeginScene(IDirect3DDevice9* pDevice)
{
	return pBeginScene(pDevice);
}
bool coordinateToggleOn = false;
HRESULT __stdcall DetourEndScene(IDirect3DDevice9* pDevice)
{
	int padding = 2;
	int rectx1 = 50, rectx2 = 270, recty1 = 250, recty2 = 300;
	//D3DRECT rectangle = { rectx1, recty1, rectx2, recty2 };
	//pDevice->Clear(1, &rectangle, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 0.0f, 0); // this draws a rectangle
	//if (!font)
	//	D3DXCreateFont(pDevice, 32, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &font);
	//RECT textRectangle;
	//SetRect(&textRectangle, rectx1 + padding, recty1 + padding, rectx2 - padding, recty2 - padding);
	//font->DrawText(0, "DLL ENABLED", -1, &textRectangle, DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 255, 0, 0));

	if (coordinateToggleOn) {
		getCoordinates();
		rectx1 = 100, rectx2 = 600, recty1 = 50, recty2 = 100;
		D3DRECT rectangle = { rectx1, recty1, rectx2, recty2 };
		pDevice->Clear(1, &rectangle, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 0.0f, 0); // this draws a rectangle
		if (!font)
			D3DXCreateFont(pDevice, 32, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &font);
		RECT textRectangle;
		SetRect(&textRectangle, rectx1 + padding, recty1 + padding, rectx2 - padding, recty2 - padding);
		font->DrawText(0, coordinates->c_str(), -1, &textRectangle, DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 153, 255, 153));
	}

	return pEndScene(pDevice);
}