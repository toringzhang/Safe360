#pragma once
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <Psapi.h>
using namespace std;
#pragma comment(lib,"Psapi.lib")
DWORD WINAPI ThreadProc_CPU(LPVOID lpParam);
class MemoryManager
{
public:
	MemoryManager();
	~MemoryManager();
	BOOL GetCPUStatus();
	BOOL GetMemoryStatus();

	void ClearMemory();
	
private:
	MEMORYSTATUSEX m_MemoryStatus;
};

