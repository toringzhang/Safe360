#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
using namespace std;
//启动项信息结构体
typedef struct _SELFRUNNINGINFO
{
	TCHAR wzBootName[50];			//启动项
	TCHAR wzBootOrder[255];			//启动命令
	TCHAR wzHKEY[50];				//主键信息
	TCHAR wzBootPos[255];			//子键
}SELFRUNNINGINFO, *PSELFRUNNINGINFO;


class SelfRunning
{
public:
	SelfRunning();
	~SelfRunning();
	void GetSelfRunningSoftware();

	void PrintInfo();

private:
	vector<SELFRUNNINGINFO> m_SelfRunningSoftware;
};




