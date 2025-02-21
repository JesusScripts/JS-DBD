#pragma once
#include <Windows.h>
#include <TlHelp32.h>

uintptr_t GetModuleBaseAddress(DWORD processId, const wchar_t* ModuleTarget);

uintptr_t GetBaseAddress(DWORD procId);

DWORD GetProcId(const wchar_t* procName);

void ListModules(DWORD processId);