#pragma once
#include "GameManip.h"
#include <stdio.h>
#include <string>

HANDLE ExeBaseAddress = GetModuleHandleA(0);

std::string* coordinates = new std::string();
bool isFlying = false;

float x = 0;
float y = 0;
float z = 0;


float positionCodeX = 0;
float positionCodeY = 0;
float positionCodeZ = 0;
float setHeight = 20;



void getCoordinates() {
	uintptr_t* coordinateBasePointer = (uintptr_t*)((uintptr_t)ExeBaseAddress + 0x252CBC);
	uintptr_t ModuleBaseAdrs = (DWORD&)*coordinateBasePointer;
	uintptr_t* ZCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E4);
	uintptr_t* XCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E0);
	uintptr_t* YCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E8);
	DWORD Val = (DWORD&)*ZCoord;
	z = *(float*)ZCoord;

	Val = (DWORD&)*XCoord;
	x = *(float*)XCoord;

	Val = (DWORD&)*YCoord;
	y = *(float*)YCoord;
	*coordinates = "X: " + std::to_string(x) + " Y: " + std::to_string(y) + " Z:" + std::to_string(z);
}

void addToCoordinates(float x, float y, float z) {
	uintptr_t* coordinateBasePointer = (uintptr_t*)((uintptr_t)ExeBaseAddress + 0x252CBC);
	uintptr_t ModuleBaseAdrs = (DWORD&)*coordinateBasePointer;
	uintptr_t* ZCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E4);
	uintptr_t* XCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E0);
	uintptr_t* YCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E8);
	DWORD Val = (DWORD&)*ZCoord;
	float* currentz = (float*)ZCoord;

	//uintptr_t* XCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E0);
	Val = (DWORD&)*XCoord;
	float* currentx = (float*)XCoord;

	//uintptr_t* YCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E8);
	Val = (DWORD&)*YCoord;
	float* currenty = (float*)YCoord;

	*currentx += x;
	*currenty += y;
	*currentz += z;
}
void SetPositionCode()
{
	getCoordinates();
	positionCodeX = x;
	positionCodeY = y;
	positionCodeZ = z;
}

DWORD __stdcall Fly() {

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


void GetPositionCode() {
	uintptr_t* coordinateBasePointer = (uintptr_t*)((uintptr_t)ExeBaseAddress + 0x252CBC);
	uintptr_t ModuleBaseAdrs = (DWORD&)*coordinateBasePointer;
	uintptr_t* ZCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E4);
	uintptr_t* XCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E0);
	uintptr_t* YCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E8);
	DWORD Val = (DWORD&)*ZCoord;
	float* z = (float*)ZCoord;

	//uintptr_t* XCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E0);
	Val = (DWORD&)*XCoord;
	float* x = (float*)XCoord;

	//uintptr_t* YCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E8);
	Val = (DWORD&)*YCoord;
	float* y = (float*)YCoord;

	*x = positionCodeX;
	*y = positionCodeY;
	*z = positionCodeZ;
}

void TurnOnHeroMode() {
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
}