// ME1 - binkw32 - main.cpp
#define NOMINMAX
#include "header.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <ShlObj.h>
#include <DbgHelp.h>
#include <Psapi.h>
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(linker, "/NODEFAULTLIB:libcmtd.lib")

// dllmain.cpp : Defines the entry point for the DLL application.
#define BASE_ADDR 0x400000
#define Z_COORDINATE 0x2533A0
//Not sure rn 
#define X_COORDINATE 0x248BF8 
#define Y_COORDINATE 0x248BF8

#include "Hooking.h"
#include "GameManip.h"
#include "Multiplayer.h"

#include <iostream>
#include <stdexcept>

#include <iostream>
#include <sstream>
#include <string>

#include <d3d9.h>
#include <d3dx9.h>


#pragma comment (lib, "libMinHook.x86.lib")

HINSTANCE hOriginalBink = NULL;
FARPROC p[71] = { 0 };
char exeBaseFolder[FILENAME_MAX];

FILE* Log = NULL;

// Sets exeBaseFolder to hold current executable's path, including "\"
void SetExecutableFolder()
{
	GetModuleFileName(NULL, exeBaseFolder, FILENAME_MAX);
	int x = strlen(exeBaseFolder) - 1;
	while (exeBaseFolder[x] != '\\')
		x--;
	exeBaseFolder[x + 1] = '\0';
}

void GetAddresses()
{
	hOriginalBink = LoadLibrary("binkw32_original.dll");
	if (hOriginalBink)
	{
		p[0] = GetProcAddress(hOriginalBink, "_BinkBufferBlit@12");
		p[1] = GetProcAddress(hOriginalBink, "_BinkBufferCheckWinPos@12");
		p[2] = GetProcAddress(hOriginalBink, "_BinkBufferClear@8");
		p[3] = GetProcAddress(hOriginalBink, "_BinkBufferClose@4");
		p[4] = GetProcAddress(hOriginalBink, "_BinkBufferGetDescription@4");
		p[5] = GetProcAddress(hOriginalBink, "_BinkBufferGetError@0");
		p[6] = GetProcAddress(hOriginalBink, "_BinkBufferLock@4");
		p[7] = GetProcAddress(hOriginalBink, "_BinkBufferOpen@16");
		p[8] = GetProcAddress(hOriginalBink, "_BinkBufferSetDirectDraw@8");
		p[9] = GetProcAddress(hOriginalBink, "_BinkBufferSetHWND@8");
		p[10] = GetProcAddress(hOriginalBink, "_BinkBufferSetOffset@12");
		p[11] = GetProcAddress(hOriginalBink, "_BinkBufferSetResolution@12");
		p[12] = GetProcAddress(hOriginalBink, "_BinkBufferSetScale@12");
		p[13] = GetProcAddress(hOriginalBink, "_BinkBufferUnlock@4");
		p[14] = GetProcAddress(hOriginalBink, "_BinkCheckCursor@20");
		p[15] = GetProcAddress(hOriginalBink, "_BinkClose@4");
		p[16] = GetProcAddress(hOriginalBink, "_BinkCloseTrack@4");
		p[17] = GetProcAddress(hOriginalBink, "_BinkControlBackgroundIO@8");
		p[18] = GetProcAddress(hOriginalBink, "_BinkControlPlatformFeatures@8");
		p[19] = GetProcAddress(hOriginalBink, "_BinkCopyToBuffer@28");
		p[20] = GetProcAddress(hOriginalBink, "_BinkCopyToBufferRect@44");
		p[21] = GetProcAddress(hOriginalBink, "_BinkDDSurfaceType@4");
		p[22] = GetProcAddress(hOriginalBink, "_BinkDX8SurfaceType@4");
		p[23] = GetProcAddress(hOriginalBink, "_BinkDX9SurfaceType@4");
		p[24] = GetProcAddress(hOriginalBink, "_BinkDoFrame@4");
		p[25] = GetProcAddress(hOriginalBink, "_BinkGetError@0");
		p[26] = GetProcAddress(hOriginalBink, "_BinkGetFrameBuffersInfo@8");
		p[27] = GetProcAddress(hOriginalBink, "_BinkGetKeyFrame@12");
		p[28] = GetProcAddress(hOriginalBink, "_BinkGetPalette@4");
		p[29] = GetProcAddress(hOriginalBink, "_BinkGetRealtime@12");
		p[30] = GetProcAddress(hOriginalBink, "_BinkGetRects@8");
		p[31] = GetProcAddress(hOriginalBink, "_BinkGetSummary@8");
		p[32] = GetProcAddress(hOriginalBink, "_BinkGetTrackData@8");
		p[33] = GetProcAddress(hOriginalBink, "_BinkGetTrackID@8");
		p[34] = GetProcAddress(hOriginalBink, "_BinkGetTrackMaxSize@8");
		p[35] = GetProcAddress(hOriginalBink, "_BinkGetTrackType@8");
		p[36] = GetProcAddress(hOriginalBink, "_BinkGoto@12");
		p[37] = GetProcAddress(hOriginalBink, "_BinkIsSoftwareCursor@8");
		p[38] = GetProcAddress(hOriginalBink, "_BinkLogoAddress@0");
		p[39] = GetProcAddress(hOriginalBink, "_BinkNextFrame@4");
		p[40] = GetProcAddress(hOriginalBink, "_BinkOpen@8");
		p[41] = GetProcAddress(hOriginalBink, "_BinkOpenDirectSound@4");
		p[42] = GetProcAddress(hOriginalBink, "_BinkOpenMiles@4");
		p[43] = GetProcAddress(hOriginalBink, "_BinkOpenTrack@8");
		p[44] = GetProcAddress(hOriginalBink, "_BinkOpenWaveOut@4");
		p[45] = GetProcAddress(hOriginalBink, "_BinkPause@8");
		p[46] = GetProcAddress(hOriginalBink, "_BinkRegisterFrameBuffers@8");
		p[47] = GetProcAddress(hOriginalBink, "_BinkRestoreCursor@4");
		p[48] = GetProcAddress(hOriginalBink, "_BinkService@4");
		p[49] = GetProcAddress(hOriginalBink, "_BinkSetError@4");
		p[50] = GetProcAddress(hOriginalBink, "_BinkSetFrameRate@8");
		p[51] = GetProcAddress(hOriginalBink, "_BinkSetIO@4");
		p[52] = GetProcAddress(hOriginalBink, "_BinkSetIOSize@4");
		p[53] = GetProcAddress(hOriginalBink, "_BinkSetMemory@8");
		p[54] = GetProcAddress(hOriginalBink, "_BinkSetMixBinVolumes@20");
		p[55] = GetProcAddress(hOriginalBink, "_BinkSetMixBins@16");
		p[56] = GetProcAddress(hOriginalBink, "_BinkSetPan@12");
		p[57] = GetProcAddress(hOriginalBink, "_BinkSetSimulate@4");
		p[58] = GetProcAddress(hOriginalBink, "_BinkSetSoundOnOff@8");
		p[59] = GetProcAddress(hOriginalBink, "_BinkSetSoundSystem@8");
		p[60] = GetProcAddress(hOriginalBink, "_BinkSetSoundTrack@8");
		p[61] = GetProcAddress(hOriginalBink, "_BinkSetVideoOnOff@8");
		p[62] = GetProcAddress(hOriginalBink, "_BinkSetVolume@12");
		p[63] = GetProcAddress(hOriginalBink, "_BinkShouldSkip@4");
		p[64] = GetProcAddress(hOriginalBink, "_BinkWait@4");
		p[65] = GetProcAddress(hOriginalBink, "_RADTimerRead@0");
	}
}

void OpenLogFile()
{
	PWSTR docfolder;
	char logfilepath[MAX_PATH];
	if (SHGetKnownFolderPath(FOLDERID_Documents, 0, 0, &docfolder) == S_OK)
	{
		wcstombs_s(NULL, logfilepath, docfolder, MAX_PATH);
		strcat_s(logfilepath, "\\dllhook.log");
		fopen_s(&Log, logfilepath, "w");
		CoTaskMemFree(docfolder);
		return;
	}
	fopen_s(&Log, "dllhook.log", "w");
}

static const char* ExceptionCodeToString(DWORD code)
{
	switch (code) {
	case EXCEPTION_ACCESS_VIOLATION:       return "ACCESS_VIOLATION";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:  return "ARRAY_BOUNDS_EXCEEDED";
	case EXCEPTION_BREAKPOINT:             return "BREAKPOINT";
	case EXCEPTION_DATATYPE_MISALIGNMENT:  return "DATATYPE_MISALIGNMENT";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:     return "FLT_DIVIDE_BY_ZERO";
	case EXCEPTION_FLT_OVERFLOW:           return "FLT_OVERFLOW";
	case EXCEPTION_ILLEGAL_INSTRUCTION:    return "ILLEGAL_INSTRUCTION";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:     return "INT_DIVIDE_BY_ZERO";
	case EXCEPTION_INT_OVERFLOW:           return "INT_OVERFLOW";
	case EXCEPTION_PRIV_INSTRUCTION:       return "PRIV_INSTRUCTION";
	case EXCEPTION_STACK_OVERFLOW:         return "STACK_OVERFLOW";
	default:                               return "UNKNOWN";
	}
}

LONG CALLBACK CrashHandler(EXCEPTION_POINTERS* ep)
{
	// Only log fatal exceptions, not benign first-chance ones
	DWORD code = ep->ExceptionRecord->ExceptionCode;
	if (code != EXCEPTION_ACCESS_VIOLATION &&
		code != EXCEPTION_STACK_OVERFLOW &&
		code != EXCEPTION_ILLEGAL_INSTRUCTION &&
		code != EXCEPTION_INT_DIVIDE_BY_ZERO &&
		code != EXCEPTION_PRIV_INSTRUCTION &&
		code != EXCEPTION_ARRAY_BOUNDS_EXCEEDED &&
		code != EXCEPTION_FLT_DIVIDE_BY_ZERO)
		return EXCEPTION_CONTINUE_SEARCH;

	// Only log once (avoid re-entry)
	static volatile LONG logged = 0;
	if (InterlockedCompareExchange(&logged, 1, 0) != 0)
		return EXCEPTION_CONTINUE_SEARCH;

	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	char* lastSlash = strrchr(path, '\\');
	if (lastSlash) strcpy_s(lastSlash + 1, MAX_PATH - (lastSlash + 1 - path), "crashlog.txt");
	else strcpy_s(path, "crashlog.txt");

	FILE* f = nullptr;
	fopen_s(&f, path, "w");
	if (!f) return EXCEPTION_CONTINUE_SEARCH;

	EXCEPTION_RECORD* er = ep->ExceptionRecord;
	CONTEXT* ctx = ep->ContextRecord;

	fprintf(f, "=== BIONICLE HEROES CRASH LOG ===\n\n");
	fprintf(f, "Exception: 0x%08X (%s)\n", er->ExceptionCode, ExceptionCodeToString(er->ExceptionCode));
	fprintf(f, "Address:   0x%08X\n", (DWORD)(DWORD_PTR)er->ExceptionAddress);

	if (er->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && er->NumberParameters >= 2) {
		fprintf(f, "  %s address 0x%08X\n",
			er->ExceptionInformation[0] == 0 ? "Reading from" :
			er->ExceptionInformation[0] == 1 ? "Writing to" : "DEP at",
			(DWORD)er->ExceptionInformation[1]);
	}

	fprintf(f, "\n--- Registers ---\n");
	fprintf(f, "EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X\n", ctx->Eax, ctx->Ebx, ctx->Ecx, ctx->Edx);
	fprintf(f, "ESI=%08X  EDI=%08X  ESP=%08X  EBP=%08X\n", ctx->Esi, ctx->Edi, ctx->Esp, ctx->Ebp);
	fprintf(f, "EIP=%08X  EFLAGS=%08X\n", ctx->Eip, ctx->EFlags);

	fprintf(f, "\n--- Call Stack ---\n");
	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();
	SymInitialize(process, NULL, TRUE);

	STACKFRAME frame = {};
	frame.AddrPC.Offset = ctx->Eip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = ctx->Ebp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = ctx->Esp;
	frame.AddrStack.Mode = AddrModeFlat;

	for (int i = 0; i < 64; i++) {
		if (!StackWalk(IMAGE_FILE_MACHINE_I386, process, thread, &frame, ctx,
			NULL, SymFunctionTableAccess, SymGetModuleBase, NULL))
			break;
		if (frame.AddrPC.Offset == 0) break;

		DWORD moduleBase = SymGetModuleBase(process, frame.AddrPC.Offset);
		char moduleName[MAX_PATH] = "???";
		if (moduleBase) {
			GetModuleFileNameA((HMODULE)(DWORD_PTR)moduleBase, moduleName, MAX_PATH);
			char* name = strrchr(moduleName, '\\');
			if (name) memmove(moduleName, name + 1, strlen(name + 1) + 1);
		}

		fprintf(f, "  [%2d] 0x%08X  %s+0x%X\n", i,
			(DWORD)frame.AddrPC.Offset, moduleName,
			(DWORD)(frame.AddrPC.Offset - moduleBase));
	}
	SymCleanup(process);

	fprintf(f, "\n--- Stack Dump (ESP) ---\n");
	DWORD esp = ctx->Esp;
	for (int row = 0; row < 16; row++) {
		DWORD addr = esp + row * 16;
		fprintf(f, "%08X: ", addr);
		for (int col = 0; col < 16; col += 4) {
			DWORD val = 0;
			if (IsBadReadPtr((void*)(DWORD_PTR)(addr + col), 4) == 0)
				val = *(DWORD*)(DWORD_PTR)(addr + col);
			fprintf(f, "%08X ", val);
		}
		fprintf(f, "\n");
	}

	fprintf(f, "\n--- Loaded Modules ---\n");
	HMODULE modules[256];
	DWORD needed;
	if (EnumProcessModules(process, modules, sizeof(modules), &needed)) {
		int count = needed / sizeof(HMODULE);
		for (int i = 0; i < count && i < 256; i++) {
			char name[MAX_PATH];
			GetModuleFileNameA(modules[i], name, MAX_PATH);
			MODULEINFO mi = {};
			GetModuleInformation(process, modules[i], &mi, sizeof(mi));
			fprintf(f, "  0x%08X - 0x%08X  %s\n",
				(DWORD)(DWORD_PTR)mi.lpBaseOfDll, (DWORD)((DWORD_PTR)mi.lpBaseOfDll + mi.SizeOfImage), name);
		}
	}

	fclose(f);

	char msg[512];
	sprintf_s(msg, "Game crashed at 0x%08X (%s)\nCrash log saved to:\n%s",
		(DWORD)(DWORD_PTR)er->ExceptionAddress, ExceptionCodeToString(er->ExceptionCode), path);
	MessageBoxA(NULL, msg, "Bionicle Heroes - Crash", MB_OK | MB_ICONERROR);

	return EXCEPTION_CONTINUE_SEARCH;
}

HMODULE newhModule;
DWORD __stdcall DestroyConsole(LPVOID lpParameter) {
	Sleep(100);
	FreeConsole();
	FreeLibraryAndExitThread(newhModule, 0);
}

DWORD WINAPI MainMenu(LPVOID lpParam)
{
	try {
		GetAddresses();
		if (hOriginalBink)
		{
			printf("Addresses loaded from binkw23.dll - OK\n");
		}
		else
		{
			printf("Error loading binkw23.dll!\n");
			return 0;
		}
		SetExecutableFolder();
		if (Log)
			fclose(Log);

		GUI::Initialize();
		Multiplayer::Init();
		Hooking::Init();
	}
	catch (const std::exception& error) {
		MessageBeep(MB_ICONERROR);
		MessageBox(0, error.what(), "DLL Hook Error", MB_OK | MB_ICONEXCLAMATION);
		Multiplayer::Shutdown();
		Hooking::Destroy();
		GUI::Destroy();
	}
	
	return 0;
}


// _BinkBufferBlit@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferBlit(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[0 * 4];
	}
}

// _BinkBufferCheckWinPos@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferCheckWinPos(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[1 * 4];
	}
}

// _BinkBufferClear@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferClear(int a1, int a2)
{
	__asm
	{
		jmp p[2 * 4];
	}
}

// _BinkBufferClose@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferClose(int a1)
{
	__asm
	{
		jmp p[3 * 4];
	}
}

// _BinkBufferGetDescription@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferGetDescription(int a1)
{
	__asm
	{
		jmp p[4 * 4];
	}
}

// _BinkBufferGetError@0
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferGetError()
{
	__asm
	{
		jmp p[5 * 4];
	}
}

// _BinkBufferLock@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferLock(int a1)
{
	__asm
	{
		jmp p[6 * 4];
	}
}

// _BinkBufferOpen@16
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferOpen(int a1, int a2, int a3, int a4)
{
	__asm
	{
		jmp p[7 * 4];
	}
}

// _BinkBufferSetDirectDraw@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferSetDirectDraw(int a1, int a2)
{
	__asm
	{
		jmp p[8 * 4];
	}
}

// _BinkBufferSetHWND@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferSetHWND(int a1, int a2)
{
	__asm
	{
		jmp p[9 * 4];
	}
}

// _BinkBufferSetOffset@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferSetOffset(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[10 * 4];
	}
}

// _BinkBufferSetResolution@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferSetResolution(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[11 * 4];
	}
}

// _BinkBufferSetScale@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferSetScale(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[12 * 4];
	}
}

// _BinkBufferUnlock@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferUnlock(int a1)
{
	__asm
	{
		jmp p[13 * 4];
	}
}

// _BinkCheckCursor@20
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkCheckCursor(int a1, int a2, int a3, int a4, int a5)
{
	__asm
	{
		jmp p[14 * 4];
	}
}

// _BinkClose@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkClose(int a1)
{
	__asm
	{
		jmp p[15 * 4];
	}
}

// _BinkCloseTrack@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkCloseTrack(int a1)
{
	__asm
	{
		jmp p[16 * 4];
	}
}

// _BinkControlBackgroundIO@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkControlBackgroundIO(int a1, int a2)
{
	__asm
	{
		jmp p[17 * 4];
	}
}

// _BinkControlPlatformFeatures@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkControlPlatformFeatures(int a1, int a2)
{
	__asm
	{
		jmp p[18 * 4];
	}
}

// _BinkCopyToBuffer@28
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkCopyToBuffer(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
	__asm
	{
		jmp p[19 * 4];
	}
}

// _BinkCopyToBufferRect@44
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkCopyToBufferRect(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11)
{
	__asm
	{
		jmp p[20 * 4];
	}
}

// _BinkDDSurfaceType@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkDDSurfaceType(int a1)
{
	__asm
	{
		jmp p[21 * 4];
	}
}

// _BinkDX8SurfaceType@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkDX8SurfaceType(int a1)
{
	__asm
	{
		jmp p[22 * 4];
	}
}

// _BinkDX9SurfaceType@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkDX9SurfaceType(int a1)
{
	__asm
	{
		jmp p[23 * 4];
	}
}

// _BinkDoFrame@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkDoFrame(int a1)
{
	__asm
	{
		jmp p[24 * 4];
	}
}

// _BinkGetError@0
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetError()
{
	__asm
	{
		jmp p[25 * 4];
	}
}

// _BinkGetFrameBuffersInfo@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetFrameBuffersInfo(int a1, int a2)
{
	__asm
	{
		jmp p[26 * 4];
	}
}

// _BinkGetKeyFrame@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetKeyFrame(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[27 * 4];
	}
}

// _BinkGetPalette@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetPalette(int a1)
{
	__asm
	{
		jmp p[28 * 4];
	}
}

// _BinkGetRealtime@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetRealtime(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[29 * 4];
	}
}

// _BinkGetRects@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetRects(int a1, int a2)
{
	__asm
	{
		jmp p[30 * 4];
	}
}

// _BinkGetSummary@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetSummary(int a1, int a2)
{
	__asm
	{
		jmp p[31 * 4];
	}
}

// _BinkGetTrackData@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetTrackData(int a1, int a2)
{
	__asm
	{
		jmp p[32 * 4];
	}
}

// _BinkGetTrackID@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetTrackID(int a1, int a2)
{
	__asm
	{
		jmp p[33 * 4];
	}
}

// _BinkGetTrackMaxSize@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetTrackMaxSize(int a1, int a2)
{
	__asm
	{
		jmp p[34 * 4];
	}
}

// _BinkGetTrackType@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetTrackType(int a1, int a2)
{
	__asm
	{
		jmp p[35 * 4];
	}
}

// _BinkGoto@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGoto(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[36 * 4];
	}
}

// _BinkIsSoftwareCursor@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkIsSoftwareCursor(int a1, int a2)
{
	__asm
	{
		jmp p[37 * 4];
	}
}

// _BinkLogoAddress@0
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkLogoAddress()
{
	__asm
	{
		jmp p[38 * 4];
	}
}

// _BinkNextFrame@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkNextFrame(int a1)
{
	__asm
	{
		jmp p[39 * 4];
	}
}

// _BinkOpen@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkOpen(int a1, int a2)
{
	__asm
	{
		jmp p[40 * 4];
	}
}

// _BinkOpenDirectSound@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkOpenDirectSound(int a1)
{
	__asm
	{
		jmp p[41 * 4];
	}
}

// _BinkOpenMiles@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkOpenMiles(int a1)
{
	__asm
	{
		jmp p[42 * 4];
	}
}

// _BinkOpenTrack@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkOpenTrack(int a1, int a2)
{
	__asm
	{
		jmp p[43 * 4];
	}
}

// _BinkOpenWaveOut@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkOpenWaveOut(int a1)
{
	__asm
	{
		jmp p[44 * 4];
	}
}

// _BinkPause@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkPause(int a1, int a2)
{
	__asm
	{
		jmp p[45 * 4];
	}
}

// _BinkRegisterFrameBuffers@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkRegisterFrameBuffers(int a1, int a2)
{
	__asm
	{
		jmp p[46 * 4];
	}
}

// _BinkRestoreCursor@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkRestoreCursor(int a1)
{
	__asm
	{
		jmp p[47 * 4];
	}
}

// _BinkService@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkService(int a1)
{
	__asm
	{
		jmp p[48 * 4];
	}
}

// _BinkSetError@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetError(int a1)
{
	__asm
	{
		jmp p[49 * 4];
	}
}

// _BinkSetFrameRate@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetFrameRate(int a1, int a2)
{
	__asm
	{
		jmp p[50 * 4];
	}
}

// _BinkSetIO@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetIO(int a1)
{
	__asm
	{
		jmp p[51 * 4];
	}
}

// _BinkSetIOSize@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetIOSize(int a1)
{
	__asm
	{
		jmp p[52 * 4];
	}
}

// _BinkSetMemory@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetMemory(int a1, int a2)
{
	__asm
	{
		jmp p[53 * 4];
	}
}

// _BinkSetMixBinVolumes@20
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetMixBinVolumes(int a1, int a2, int a3, int a4, int a5)
{
	__asm
	{
		jmp p[54 * 4];
	}
}

// _BinkSetMixBins@16
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetMixBins(int a1, int a2, int a3, int a4)
{
	__asm
	{
		jmp p[55 * 4];
	}
}

// _BinkSetPan@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetPan(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[56 * 4];
	}
}

// _BinkSetSimulate@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetSimulate(int a1)
{
	__asm
	{
		jmp p[57 * 4];
	}
}

// _BinkSetSoundOnOff@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetSoundOnOff(int a1, int a2)
{
	__asm
	{
		jmp p[58 * 4];
	}
}

// _BinkSetSoundSystem@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetSoundSystem(int a1, int a2)
{
	__asm
	{
		jmp p[59 * 4];
	}
}

// _BinkSetSoundTrack@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetSoundTrack(int a1, int a2)
{
	__asm
	{
		jmp p[60 * 4];
	}
}

// _BinkSetVideoOnOff@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetVideoOnOff(int a1, int a2)
{
	__asm
	{
		jmp p[61 * 4];
	}
}

// _BinkSetVolume@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetVolume(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[62 * 4];
	}
}

// _BinkShouldSkip@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkShouldSkip(int a1)
{
	__asm
	{
		jmp p[63 * 4];
	}
}

// _BinkWait@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkWait(int a1)
{
	__asm
	{
		jmp p[64 * 4];
	}
}

// _RADTimerRead@0
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall RADTimerRead()
{
	__asm
	{
		jmp p[65 * 4];
	}
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hModule);

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		AddVectoredExceptionHandler(1, CrashHandler);
		Hooking::InitInputHook(hModule);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainMenu, hModule, 0, nullptr);
	}
	if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		FreeLibrary(hOriginalBink);
	}
	return TRUE;
}


