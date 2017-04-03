#include "stdafx.h"
#include "MemoryManager.h"


MemoryManager::MemoryManager()
{
	m_MemoryStatus = { sizeof(MEMORYSTATUSEX) };
}


MemoryManager::~MemoryManager()
{
}

BOOL MemoryManager::GetCPUStatus()
{
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc_CPU, this, 0, NULL);
	if (hThread)
	{
		return TRUE;
	}
	return FALSE;
}

DWORD WINAPI ThreadProc_CPU(LPVOID lpParam)
{
	HANDLE hEvent;
	FILETIME preidleTime, prekernelTime, preuserTime;
	FILETIME idleTime, kernelTime, userTime;
	DOUBLE FreeTime1;
	DOUBLE FreeTime2;
	DOUBLE KernelTime1;
	DOUBLE KernelTime2;
	DOUBLE UserTime1;
	DOUBLE UserTime2;
	DWORD  CPU;

	//1.第一次获取处理器时间
	//检索系统时序信息。在多处理器系统上，返回的值是所有处理器之间指定时间的总和。
	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	preidleTime = idleTime;
	prekernelTime = kernelTime;
	preuserTime = userTime;
	//2.创建内核对象，并且等待1000毫秒
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	while (1)
	{
		WaitForSingleObject(hEvent, 1000);
		//3.第二次获取处理器时间
		GetSystemTimes(&idleTime, &kernelTime, &userTime);
		//4.转换时间
		FreeTime1 = (DOUBLE)(preidleTime.dwHighDateTime*4.294967296E9) +
			(DOUBLE)preidleTime.dwLowDateTime;
		FreeTime2 = (DOUBLE)(idleTime.dwHighDateTime*4.294967296E9) +
			(DOUBLE)idleTime.dwLowDateTime;
		KernelTime1 = (DOUBLE)(prekernelTime.dwHighDateTime*4.294967296E9) +
			(DOUBLE)prekernelTime.dwLowDateTime;
		KernelTime2 = (DOUBLE)(kernelTime.dwHighDateTime*4.294967296E9) +
			(DOUBLE)kernelTime.dwLowDateTime;
		UserTime1 = (DOUBLE)(preuserTime.dwHighDateTime*4.294967296E9) +
			(DOUBLE)preuserTime.dwLowDateTime;
		UserTime2 = (DOUBLE)(userTime.dwHighDateTime*4.294967296E9) +
			(DOUBLE)userTime.dwLowDateTime;
		//5.计算CPU使用率
		CPU = (DWORD)(100.0 - (FreeTime2 - FreeTime1) /
			(KernelTime2 - KernelTime1 + UserTime2 - UserTime1)*100.0);
		//6.向主窗口发送消息，更新CPU使用率
		printf("CPU使用率:%d%%\r\n",CPU);
		//7.重新复制，循环刷新CPU使用率
		preidleTime   = idleTime;
		prekernelTime = kernelTime;
		preuserTime   = userTime;
	}
	return 0;
}

BOOL MemoryManager::GetMemoryStatus()
{
	
	if (!GlobalMemoryStatusEx(&m_MemoryStatus))
	{
		printf("Cannot Get Memory Information!\r\n");
		return FALSE;
	}
	printf("-------------内存信息-----------\r\n");
	printf("物理内存总数：%f GB\r\n",(float)(m_MemoryStatus.ullTotalPhys/1024.0f/1024.0f/1024.0f));
	printf("空闲物理内存：%f GB\r\n", (float)(m_MemoryStatus.ullAvailPhys / 1024.0f / 1024.0f / 1024.0f));
	printf("已使用百分比：%d%% \r\n", m_MemoryStatus.dwMemoryLoad);
	printf("分页文件总数：%f GB\r\n", (float)(m_MemoryStatus.ullTotalPageFile / 1024.0f / 1024.0f / 1024.0f));
	printf("空闲分页文件：%f GB\r\n", (float)(m_MemoryStatus.ullAvailPageFile / 1024.0f / 1024.0f / 1024.0f));
	printf("虚拟内存总数：%f GB\r\n", (float)(m_MemoryStatus.ullTotalVirtual / 1024.0f / 1024.0f / 1024.0f));
	printf("空闲虚拟内存：%f GB\r\n", (float)(m_MemoryStatus.ullAvailVirtual / 1024.0f / 1024.0f / 1024.0f));
	printf("--------------------------------\r\n");
	return TRUE;
}

void MemoryManager::ClearMemory()
{
	printf("正在清理内存...\r\n");

	HANDLE ProcessSnapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (ProcessSnapHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}

	PROCESSENTRY32 ProcessInfo;
	ProcessInfo.dwSize = sizeof(ProcessInfo);

	//获取系统中第一个进程的信息  
	BOOL Status = Process32First(ProcessSnapHandle, &ProcessInfo);
	while (Status)
	{
		HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE,
			ProcessInfo.th32ProcessID);
		if (ProcessHandle)
		{
			//设置进程工作区大小
			SetProcessWorkingSetSize(ProcessHandle, -1, -1);
			//尽可能多的将指定进程的页面从工作区移除
			EmptyWorkingSet(ProcessHandle);
			CloseHandle(ProcessHandle);
		}
		//获取下一个进程的信息  
		Status = Process32Next(ProcessSnapHandle, &ProcessInfo);
	}
}