#pragma once
#include <MinHook.h>
#include <Windows.h>
#include <d3d9.h>
#include <string>
#include <d3dx9.h>
#include "intrin.h"

extern const HANDLE ExeBaseAddress;
extern std::string coordinates;

std::string TurnOnHeroMode();
void addToCoordinates(float x, float y, float z);
std::string getCoordinates();
DWORD __stdcall Fly();
void SetDebugFlag(int flagIndex);
void Patch(void* address, std::initializer_list<uint8_t> list);
void WriteInProcessByte(DWORD Address, unsigned long long value);
unsigned long long* ReadInProcessByte(DWORD Address);
void GetPositionCode();
void setByte(uint64_t* bytes, uint8_t byte, int pos);