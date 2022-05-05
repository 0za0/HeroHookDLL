// dllmain.cpp : Defines the entry point for the DLL application.
#define BASE_ADDR 0x400000
#define Z_COORDINATE 0x2533A0
//Not sure rn 
#define X_COORDINATE 0x248BF8 
#define Y_COORDINATE 0x248BF8

#include <MinHook.h>
#include <Windows.h>
#include <iostream>
#include <stdexcept>

#include <iostream>
#include <sstream>
#include <string>

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment (lib, "libMinHook.x86.lib")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")



typedef HRESULT(__stdcall* endScene)(IDirect3DDevice9* pDevice);
endScene pEndScene; //Original Pointer

typedef HRESULT(__stdcall* beginScene)(IDirect3DDevice9* pDevice);
beginScene pBeginScene; //Original Pointer


LPD3DXFONT font;
std::string* coordinates = new std::string();
bool thing = false;
bool isFlying = false;

float x = 0;
float y = 0;
float z = 0;

float _x = 0;
float _y = 0;
float _z = 0;
float setHeight = 20;


//AddCoordStuff
float addX = 10.0;
float addY = 10.0;
float addZ = 10.0;

void addToCoordinates(float x, float y, float z) 
{
	HANDLE ExeBaseAddress = GetModuleHandleA(0);
	uintptr_t* p = (uintptr_t*)((uintptr_t)ExeBaseAddress + 0x252CBC);
	uintptr_t ModuleBaseAdrs = (DWORD&)*p;

	uintptr_t* ZCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E4);
	DWORD Val = (DWORD&)*ZCoord;
	float* currentz = (float*)ZCoord;

	uintptr_t* XCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E0);
	Val = (DWORD&)*XCoord;
	float* currentx = (float*)XCoord;

	uintptr_t* YCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E8);
	Val = (DWORD&)*YCoord;
	float* currenty = (float*)YCoord;

	*currentx += x;
	*currenty += y;
	*currentz += z;
}

void getCoordinates() {

	HANDLE ExeBaseAddress = GetModuleHandleA(0);
	uintptr_t* p = (uintptr_t*)((uintptr_t)ExeBaseAddress + 0x252CBC);
	uintptr_t ModuleBaseAdrs = (DWORD&)*p;

	uintptr_t* ZCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E4);
	DWORD Val = (DWORD&)*ZCoord;
	z = *(float*)ZCoord;

	uintptr_t* XCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E0);
	Val = (DWORD&)*XCoord;
	x = *(float*)XCoord;

	uintptr_t* YCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E8);
	Val = (DWORD&)*YCoord;
	y = *(float*)YCoord;


	*coordinates = "X: " + std::to_string(x) + " Y: " + std::to_string(y) + " Z:" + std::to_string(z);

	//std::cout << "X: " << std::to_string(x) << " Y: " << std::to_string(y) << " Z:" << std::to_string(z);
}

DWORD __stdcall Fly() {

	HANDLE ExeBaseAddress = GetModuleHandleA(0);
	uintptr_t* p = (uintptr_t*)((uintptr_t)ExeBaseAddress + 0x252CBC);
	uintptr_t ModuleBaseAdrs = (DWORD&)*p;

	uintptr_t* ZCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E4);
	DWORD Val = (DWORD&)*ZCoord;
	float* z = (float*)ZCoord;

	while (1) {
		
		*z = setHeight;
		if (GetAsyncKeyState(VK_PRIOR)) {
			Sleep(100);
			setHeight += 1;
		}
		if (GetAsyncKeyState(VK_NEXT)) {
			Sleep(100);
			setHeight -= 1;
		}
		if (GetAsyncKeyState(VK_BACK))
			break;
	}
	return 0;
}
HRESULT __stdcall DetourBeginScene(IDirect3DDevice9* pDevice)
{
	//pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	return pBeginScene(pDevice);

}

HRESULT __stdcall DetourEndScene(IDirect3DDevice9* pDevice)
{
	getCoordinates();
	int padding = 2;
	int rectx1 = 100, rectx2 = 600, recty1 = 50, recty2 = 100;
	D3DRECT rectangle = { rectx1, recty1, rectx2, recty2 };
	pDevice->Clear(1, &rectangle, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 0.0f, 0); // this draws a rectangle
	if (!font)
		D3DXCreateFont(pDevice, 32, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &font);

	RECT textRectangle;
	SetRect(&textRectangle, rectx1 + padding, recty1 + padding, rectx2 - padding, recty2 - padding);
	font->DrawText(0, coordinates->c_str(), -1, &textRectangle, DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(255, 153, 255, 153));


	return pEndScene(pDevice);
}
void InitMinHook() {
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

	if (MH_Initialize())
		throw std::runtime_error("Couldn't initialize MinHook");

	if (MH_CreateHook(vTable[42],
		&DetourEndScene,
		reinterpret_cast<void**>(&pEndScene)) != MH_OK)
		throw std::runtime_error("Bruh");

	if (MH_CreateHook(vTable[41],
		&DetourBeginScene,
		reinterpret_cast<void**>(&pBeginScene)) != MH_OK)
		throw std::runtime_error("Bruh");
}
HMODULE newhModule;
DWORD __stdcall DestroyConsole(LPVOID lpParameter) {
	Sleep(100);
	FreeLibraryAndExitThread(newhModule, 0);
}
DWORD TurnOnHeroMode() {
	HANDLE ExeBaseAddress = GetModuleHandleA(0);
	uintptr_t* p = (uintptr_t*)((uintptr_t)ExeBaseAddress + 0x29B1BC);
	uintptr_t ModuleBaseAdrs = (DWORD&)*p;

	uintptr_t* HeroMode = (uintptr_t*)(ModuleBaseAdrs + 0xCD84);
	DWORD Val = (DWORD&)*HeroMode;
	UINT* z = (UINT*)HeroMode;

	if (*z == 1) {
		*z = 0;
		printf("Hero Mode Toggle OFF!\n");
	}
	else {
		*z = 1;
		printf("Hero Mode Toggle ON!\n");
	}


	return 0;
}
//https://stackoverflow.com/questions/15543571/allocconsole-not-displaying-cout 
DWORD WINAPI MainMenu() {
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	std::cout << "Bootleg DLL Injected Console" << std::endl;
	printf("NOTE: CLOSING THE CONSOLE WITH THE X CLOSES THE GAME ... so dont unless you want to idk man you do you\n");

	printf("CONTROLS:\n");
	printf("The \| Key enables 'flight'\n");
	printf("While flying page up and page down add or decrease height\n");
	printf("Number 1 Toggles Hero mode\n");
	printf("Number 2 Sets your location\n");
	printf("Number 3 Teleports you to the set location\n");
	printf("\n\nShort Range Teleport Keybinds\n\n");
	printf("\t8 Forward\n4 Left\t5  Jump   6 Right\n\t2 Backward");
	printf("\n\n\nThe DELETE key unhooks the dll without closing the game!!\n");
	InitMinHook();
	while (1) {
		Sleep(100);
		if (GetAsyncKeyState(VK_DELETE)) {
			MH_DisableHook(MH_ALL_HOOKS);
			break;
		}
		if (GetAsyncKeyState(VK_NUMPAD1)) {
			Sleep(100);
			if (!thing) {
				if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
					CreateThread(0, 0, DestroyConsole, 0, 0, 0);//
				thing = true;
			}
			else {
				MH_DisableHook(MH_ALL_HOOKS);
				thing = false;
			}
		}
		if (GetAsyncKeyState(0x32))//2 key
		{
			getCoordinates();
			printf("Set position\n");
			_x = x;
			_y = y;
			_z = z;

		}
		if (GetAsyncKeyState(0x33))//3 key
		{
			HANDLE ExeBaseAddress = GetModuleHandleA(0);
			uintptr_t* p = (uintptr_t*)((uintptr_t)ExeBaseAddress + 0x252CBC);
			uintptr_t ModuleBaseAdrs = (DWORD&)*p;

			uintptr_t* ZCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E4);
			DWORD Val = (DWORD&)*ZCoord;
			float* z = (float*)ZCoord;

			uintptr_t* XCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E0);
			Val = (DWORD&)*XCoord;
			float* x = (float*)XCoord;

			uintptr_t* YCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E8);
			Val = (DWORD&)*YCoord;
			float* y = (float*)YCoord;

			printf("Reading position\n");
			*x = _x;
			*y = _y;
			*z = _z;
		}
		if (GetAsyncKeyState(0x31)) {
			Sleep(100); //No spam ... stop
			TurnOnHeroMode();
		}
		if (GetAsyncKeyState(VK_OEM_5)) { //THe fuck
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Fly, 0, 0, 0);//
		}

		//Movement stuff
		if (GetAsyncKeyState(VK_NUMPAD8))
			addToCoordinates(0, addY, 0);
		
		if (GetAsyncKeyState(VK_NUMPAD2))
			addToCoordinates(0, -addY, 0);
		
		if (GetAsyncKeyState(VK_NUMPAD4))
			addToCoordinates(-addX, 0, 0);

		if (GetAsyncKeyState(VK_NUMPAD6))
			addToCoordinates(addX, 0, 0);

		if (GetAsyncKeyState(VK_NUMPAD5))
			addToCoordinates(0, 0, addZ);
		

	}
	fclose(fp);
	FreeConsole();
	// Close the shit
	CreateThread(0, 0, DestroyConsole, 0, 0, 0);
	return 0;
}
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		newhModule = hModule;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainMenu, NULL, 0, NULL);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

