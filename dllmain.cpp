// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <cstdint>
#include <thread>
#include <vector>


uintptr_t ResolvePtr(uintptr_t base, const std::initializer_list<uintptr_t>& offsets)
{
	uintptr_t addr = base;
	for (auto off : offsets)
	{
		addr = *reinterpret_cast<uintptr_t*>(addr + off); // read next pointer
		if (!addr) return 0;
	}
	return addr;
}



DWORD WINAPI MainThread(HMODULE hModule) {

	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONIN$", "r", stdin);
	freopen_s(&f, "CONOUT$", "w", stderr);

	// outputs to console
	printf("DLL injected!\n");
	printf("press N to toggle fly\n");
	printf("press m to toggle health\n");
	printf("press c to toggle instant break\n");
	printf("press L to unload dll\n");
	printf("https://github.com/table-bridge-studios\n");


	uintptr_t ModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandleW(L"Minecraft.Windows.exe"));

	// pointers and offsets outdated find new ones using cheat engine or IDA / ghidra (havent changed these in months)
	uintptr_t* player = (uintptr_t*)(ModuleBase + 0x0970AB18);
	uintptr_t* breakbase = (uintptr_t*)(ModuleBase + 0x098F4388);
	uintptr_t healthAddr = ResolvePtr(*player, { 0x100, 0x98, 0x12D0, 0x50, 0x0, 0x18 }) + 0x2FC;
	uintptr_t flyAddr = ResolvePtr(*player, { 0x108, 0xA8, 0x450, 0x50, 0x0 }) + 0x164;
	uintptr_t breakaddr = ResolvePtr(*breakbase, { 0x10, 0x18, 0x90, 0x708 }) + 0x24;

	using byte = std::uint8_t;

	//toggles
	bool flyEnabled = true;
	bool healthEnabled = true;
	bool breakEnabled = false;



	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

		// detects key press then enables cheat
		if (GetAsyncKeyState('M') & 1)
		{
			healthEnabled = !healthEnabled;
		}



		if (healthEnabled)
		{
			*reinterpret_cast<float*>(healthAddr) = 20.0f;
		}



		// detect SINGLE press of N
		if (GetAsyncKeyState('N') & 1)
		{
			flyEnabled = !flyEnabled;
		}


		if (flyEnabled)
		{
			*reinterpret_cast<byte*>(flyAddr) = flyEnabled ? 1 : 0;
		}

		if (GetAsyncKeyState('C') & 1)
		{
			breakEnabled = !breakEnabled;
			
		}


		if (breakEnabled)
		{
			*reinterpret_cast<float*>(breakaddr) = 1.0f;
		}

		if (GetAsyncKeyState('L') & 1)
		{
			break;
		}

	}
	printf("DLL unloaded!\n");
	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

