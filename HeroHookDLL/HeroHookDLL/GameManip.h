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

void SetPositionCode();
void GetPositionCode();