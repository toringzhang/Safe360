// Safe666.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<Windows.h>
#include "ViresScan.h"
#include "SoftwareManager.h"
#include "MemoryManager.h"
#include "SelfRunning.h"
#include "Services.h"
#include "Rubbish.h"

void TestVireScan();
void TestMemoryManager();
void TestSelfRunning();
void TestServices();
void TestRubbish();
int main()
{
	
	//TestVireScan();
	//TestMemoryManager();
	//TestSelfRunning();
	//TestServices();
	TestRubbish();
    return 0;
}

void TestVireScan()
{
	CViresScan VireScan;
	VireScan.StartScan();
	//获得软件信息
	SoftwareManager sm;
	sm.GetSoftwareInfo();
	sm.PrintSoftwareInfo();
}

void TestMemoryManager()
{
	MemoryManager mm;
	mm.GetMemoryStatus();
	mm.ClearMemory();
	mm.GetMemoryStatus();
	mm.GetCPUStatus();
	getchar();
}

void TestSelfRunning()
{
	SelfRunning sr;
	sr.GetSelfRunningSoftware();
	sr.PrintInfo();
}

void TestServices()
{
	Services ss;
	ss.GetServicesInfo();
	ss.PrintServicesInfo();
}

void TestRubbish()
{
	Rubbish rs;
	rs.StartScan();
	rs.StartCleanThread();
	getchar();
	getchar();
	rs.PrintRubbishInfo();


}