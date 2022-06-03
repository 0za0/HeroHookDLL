#pragma once
#include <MinHook.h>
#include <Windows.h>
#include <d3d9.h>
#include <string>

#include <d3dx9.h>

extern HANDLE ExeBaseAddress;
extern std::string* coordinates;

void TurnOnHeroMode();
void addToCoordinates(float x, float y, float z);
void getCoordinates();
DWORD __stdcall Fly();
void DisableGravity();
void EnableGravity();
void Patch(void* address, std::initializer_list<uint8_t> list);
void WriteInProcessByte(DWORD Address, unsigned long long value);
unsigned long long* ReadInProcessByte(DWORD Address);
void SetPositionCode();
void GetPositionCode();
void setByte(uint64_t* bytes, uint8_t byte, int pos);