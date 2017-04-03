#pragma once
#include<Windows.h>
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;

#define Win32PATH _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
#define Win64PATH _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall")

//软件信息结构体
typedef struct _SOFTWAREINFO
{
	TCHAR wzSoftName[50];						//软件名称
	TCHAR wzSoftVersion[50];					//软件版本号
	TCHAR wzSoftPublisher[50];					//软件发布厂商
	TCHAR wzSoftDate[20];						//软件安装日期
	TCHAR wzSoftInstallPath[MAX_PATH];			//软件安装路径
	TCHAR wzSoftUninstallPath[MAX_PATH];		//软件卸载路径
}SOFTWAREINFO, *PSOFTWAREINFO;

class SoftwareManager
{
public:
	SoftwareManager();
	~SoftwareManager();

	void GetSoftwareInfo();
	BOOL IsX64();
	BOOL PrintSoftwareInfo();
private:
	vector<SOFTWAREINFO> m_SoftwareInfo;
};

