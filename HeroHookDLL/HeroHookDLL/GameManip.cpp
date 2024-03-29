#pragma once
#include "GameManip.h"
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iterator>
#include "intrin.h"

HANDLE ExeBaseAddress = GetModuleHandleA(0);

std::string* coordinates = new std::string();
bool isFlying = false;

float x = 0;
float y = 0;
float z = 0;

//Gravity Enabled
//448B000000F49ED9
//8E8B000000F4868B
//Bionicle.exe + ABF6F
//Bionicle.exe + ABE45

//Gravity Disabled
//448B909090909090
//8E8B909090909090


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

	z = *(float*)ZCoord;
	x = *(float*)XCoord;
	y = *(float*)YCoord;
	*coordinates = "X: " + std::to_string(x) + " Y: " + std::to_string(y) + " Z:" + std::to_string(z);
}

void addToCoordinates(float x, float y, float z) {
	uintptr_t* coordinateBasePointer = (uintptr_t*)((uintptr_t)ExeBaseAddress + 0x252CBC);
	uintptr_t ModuleBaseAdrs = (DWORD&)*coordinateBasePointer;
	uintptr_t* ZCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E4);
	uintptr_t* XCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E0);
	uintptr_t* YCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E8);
	float* currentz = (float*)ZCoord;
	//uintptr_t* XCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E0);
	float* currentx = (float*)XCoord;
	//uintptr_t* YCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E8);
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

void Patch(void* address, std::initializer_list<uint8_t> list) {
	uint8_t* const addr = (uint8_t*)address;
	std::copy(list.begin(), list.end(), stdext::make_checked_array_iterator(addr, list.size()));
}

void EnableGravity() {
	std::initializer_list<uint8_t> list = { 0xD9, 0x9E ,0xF4 ,0x00 ,0x00 ,0x00 };
	DWORD		dwProtect[2];
	VirtualProtect((void*)0x004ABF6F, list.size(), PAGE_EXECUTE_READWRITE, &dwProtect[0]);
	Patch((void*)0x004ABF6F, std::move(list));
	VirtualProtect((void*)0x004ABF6F, list.size(), dwProtect[0], &dwProtect[1]);

	//8E8B000000F486
	std::initializer_list<uint8_t> listt = { 0x8B, 0x86 ,0xF4 ,0x00 ,0x00 ,0x00 };
	DWORD		dwProtectt[2];
	VirtualProtect((void*)0x004ABE45, listt.size(), PAGE_EXECUTE_READWRITE, &dwProtectt[0]);
	Patch((void*)0x004ABE45, std::move(listt));
	VirtualProtect((void*)0x004ABE45, listt.size(), dwProtectt[0], &dwProtectt[1]);
}

void DisableGravity() {

	//Gravity Enabled
//448B000000F49ED9
//8E8B000000F4868B

	DWORD		dwProtect[2];
	VirtualProtect((void*)0x004ABF6F, 6, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
	memset((void*)(0x004ABF6F), 0x90, 6);
	VirtualProtect((void*)0x004ABF6F, 6, dwProtect[0], &dwProtect[1]);


	DWORD		dwProtectt[2];
	VirtualProtect((void*)0x004ABE45, 6, PAGE_EXECUTE_READWRITE, &dwProtectt[0]);
	memset((void*)(0x004ABE45), 0x90, 6);
	VirtualProtect((void*)0x004ABE45, 6, dwProtectt[0], &dwProtectt[1]);

	//printf("Trying to run the function");
	//typedef void __cdecl func(int a);
	////func* f = (func*)(0x004FF8CF);
	//func* f=(func*)(0x00455AE0);

	//f(10);
}

bool GetDebugFlag(int flagIndex) {
	//Get the pointer inside RAM
	int offset = *(int*)(0x00601660);
	//Apply the offset and get the value at the offset position
	int result = *(((char*)offset) + 0x8 * flagIndex);
	//Get the other pointer inside RAM
	offset = *(int*)(0x0064E1F8);
	//apply the offset and get its address
	int* flagLoc = (int*)(((char*)offset) + result);
	uintptr_t* flag = (uintptr_t*)(flagLoc);
	return *flag != 0;
	
}


void SetDebugFlag(int flagIndex) {
	//Get the pointer inside RAM
	int offset = *(int*)(0x00601660);
	//Apply the offset and get the value at the offset position
	int result = *(((char*)offset) + 0x8 * flagIndex);
	//Get the other pointer inside RAM
	offset = *(int*)(0x0064E1F8);
	//apply the offset and get its address
	int* flagLoc = (int*)(((char*)offset) + result);


	DWORD        dwProtect[2];
	VirtualProtect((void*)(flagLoc), 1, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
	uintptr_t* flag = (uintptr_t*)(flagLoc);
	*flag = *flag == 0;
	VirtualProtect((void*)(flagLoc), 1, dwProtect[0], &dwProtect[1]);

}

void setByte(uint64_t* bytes, uint8_t byte, int pos) {
	*bytes &= ~((uint64_t)0xff << (8 * pos)); // Clear the current byte
	*bytes |= ((uint64_t)byte << (8 * pos)); // Set the new byte
}
unsigned long long* ReadInProcessByte(DWORD Address)
{
	return (unsigned long long*)Address;
}

void WriteInProcessByte(DWORD Address, unsigned long long Value)
{
	*(unsigned long long*)Address = Value;
}

DWORD __stdcall Fly() {
	uintptr_t* p = (uintptr_t*)((uintptr_t)ExeBaseAddress + 0x252CBC);
	uintptr_t ModuleBaseAdrs = (DWORD&)*p;
	uintptr_t* ZCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E4);
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
		if (GetAsyncKeyState(VK_BACK)) {
			EnableGravity();
			break;
		}
	}
	return 0;
}


void GetPositionCode() {
	uintptr_t* coordinateBasePointer = (uintptr_t*)((uintptr_t)ExeBaseAddress + 0x252CBC);
	uintptr_t ModuleBaseAdrs = (DWORD&)*coordinateBasePointer;
	uintptr_t* ZCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E4);
	uintptr_t* XCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E0);
	uintptr_t* YCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E8);
	//DWORD Val = (DWORD&)*ZCoord;
	float* z = (float*)ZCoord;

	//uintptr_t* XCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E0);
	//Val = (DWORD&)*XCoord;
	float* x = (float*)XCoord;

	//uintptr_t* YCoord = (uintptr_t*)(ModuleBaseAdrs + 0x6E8);
	//Val = (DWORD&)*YCoord;
	float* y = (float*)YCoord;

	*x = positionCodeX;
	*y = positionCodeY;
	*z = positionCodeZ;
}

std::string TurnOnHeroMode() {
	uintptr_t* p = (uintptr_t*)((uintptr_t)ExeBaseAddress + 0x29B1BC);
	uintptr_t ModuleBaseAdrs = (DWORD&)*p;

	uintptr_t* HeroMode = (uintptr_t*)(ModuleBaseAdrs + 0xCD84);
	//DWORD Val = (DWORD&)*HeroMode;
	UINT* z = (UINT*)HeroMode;

	if (*z == 1) {
		*z = 0;
		return "Hero Mode Toggle OFF!";
	}
	else {
		*z = 1;
		return "Hero Mode Toggle ON!";
	}
}