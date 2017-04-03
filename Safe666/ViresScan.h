#pragma once
#include <Windows.h>
#include <vector>

#include "MD5.h"

class CViresScan
{
public:
	CViresScan();
	~CViresScan();
	void CheckSelect();
	void StartScan();
	void ScanSingle(WCHAR * wzPath);
	void ScanAll(WCHAR * wzPath);
	BOOL MD5Scan(WCHAR * wzPath);
	BOOL WhiteScan(WCHAR * wzPath);
private:
	
	WCHAR m_Path[MAX_PATH];  //扫描为单个文件或者全路径
	WCHAR m_Way[MAX_PATH];   //扫描为MD5或者白名单
	WCHAR m_SettingPath[MAX_PATH]; //设置文件路径
	WCHAR m_ScanPath[MAX_PATH];    //查杀路径
	WCHAR m_MD5[32];          //记录MD5的值
	std::vector<WCHAR*> m_LocalMD5;		//本地MD5库
};

