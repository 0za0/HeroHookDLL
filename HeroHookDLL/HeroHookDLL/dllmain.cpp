// dllmain.cpp : Defines the entry point for the DLL application.
#define BASE_ADDR 0x400000
#define Z_COORDINATE 0x2533A0
//Not sure rn 
#define X_COORDINATE 0x248BF8 
#define Y_COORDINATE 0x248BF8

#include "Hooking.h"
#include "GameManip.h"

#include <iostream>
#include <stdexcept>

#include <iostream>
#include <sstream>
#include <string>

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment (lib, "libMinHook.x86.lib")
bool coordinateToggle = false;

float addX = 10.0;
float addY = 10.0;
float addZ = 10.0;


HMODULE newhModule;
DWORD __stdcall DestroyConsole(LPVOID lpParameter) {
	Sleep(100);
	FreeConsole();
	FreeLibraryAndExitThread(newhModule, 0);
}

//https://stackoverflow.com/questions/15543571/allocconsole-not-displaying-cout 
DWORD WINAPI MainMenu() {
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	//freopen_s(&fp, "CONIN$", "r", stdin); Not yet
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
			CreateThread(0, 0, DestroyConsole, 0, 0, 0);
			break;
		}
		if (GetAsyncKeyState(VK_NUMPAD1)) {
			Sleep(100);
			if (!coordinateToggle) {
				if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
					CreateThread(0, 0, DestroyConsole, 0, 0, 0);
				coordinateToggle = true;
			}
			else {
				MH_DisableHook(MH_ALL_HOOKS);
				coordinateToggle = false;
			}
		}
		if (GetAsyncKeyState(0x32))//2 key
		{
			printf("Set position\n");
			SetPositionCode();
		}
		if (GetAsyncKeyState(0x33))//3 key
			GetPositionCode();

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
	// Close the thing
	CreateThread(0, 0, DestroyConsole, 0, 0, 0);
	return 0;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
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

