#include "stdafx.h"
#include "ViresScan.h"


CViresScan::CViresScan()
{
	CheckSelect();
}


CViresScan::~CViresScan()
{
}

void CViresScan::CheckSelect()
{
	
	GetCurrentDirectory(MAX_PATH, m_SettingPath);
	lstrcatW(m_SettingPath, L"\\setting.ini");
	//获得配置文件中选择的扫描方式
	GetPrivateProfileString(L"ScanWay", L"Path", NULL, m_Path, MAX_PATH, m_SettingPath);
	//获得配置文件中选择的查杀方式
	GetPrivateProfileString(L"ScanWay", L"Way", NULL, m_Way, MAX_PATH, m_SettingPath);

	//获取本地MD5库
	for (int i = 0; i < 3; i++)
	{
		WCHAR Index[10];
		wsprintf(Index, L"%d", i);
		WCHAR v1[32+1] = {0};
		GetPrivateProfileString(L"MD5", Index, NULL, v1, MAX_PATH, m_SettingPath);
		m_LocalMD5.push_back(v1);
	}
}

void CViresScan::StartScan()
{
	//单文件扫描
	if (lstrcmp(L"0", m_Path)==0)
	{
		//获得配置文件中选择的查杀路径
		GetPrivateProfileString(L"ScanPath", L"SinglePath", NULL,m_ScanPath,MAX_PATH,m_SettingPath);
		
		ScanSingle(m_ScanPath);
	}
	else
	{
		//获得配置文件中选择的查杀路径
		GetPrivateProfileString(L"ScanPath", L"Directory", NULL, m_ScanPath, MAX_PATH, m_SettingPath);
		ScanAll(m_ScanPath);
	}

}

//单文件扫描
void CViresScan::ScanSingle(WCHAR* wzPath)
{
	WIN32_FIND_DATA Win32FindData;

	DWORD dwAttributes = 0;

	FindFirstFile(wzPath, &Win32FindData);

	//判断是否是MD5查杀
	if (lstrcmp(L"0", m_Way) == 0)
	{
		//MD5查杀
		if (MD5Scan(wzPath))
		{
			printf("该文件是病毒!MD5:%S",m_MD5);
			printf(" \r\n");
		}
		else
		{
			printf("该文件不是病毒!MD5:%S", m_MD5);
			printf(" \r\n");
		}
	}
	else
	{
		//白名单查杀
		if (WhiteScan(wzPath))
		{
			
		}
	}
}

//全路径扫描
void CViresScan::ScanAll(WCHAR* wzPath)
{

}

//MD5
BOOL CViresScan::MD5Scan(WCHAR* wzPath)
{
	//LPCTSTR 转CHAR*
	int Length = WideCharToMultiByte(CP_OEMCP, NULL, wzPath, -1, NULL, 0, NULL, FALSE);
	char *v1 = new char[Length];
	WideCharToMultiByte(CP_OEMCP, NULL, wzPath, -1, v1, Length, NULL, FALSE);
	//获取MD5值
	char* MD5 = MD5FileValue(v1);
	//CHAR* 转LPCTSTR
	Length = MultiByteToWideChar(0, 0, MD5, -1, NULL, 0);
	WCHAR *v2 = new WCHAR[Length];
	MultiByteToWideChar(0, 0, MD5, -1, v2, Length);
	memcpy(m_MD5, v2, Length);
	delete[]v1;
	delete[]v2;

	//获取文件MD5信息完毕
	//本地MD5查杀		与m_LocalMD5作对比
	for (DWORD i = 0; i < m_LocalMD5.size(); i++)
	{
		if (*m_LocalMD5[i] == *m_MD5)
		{
			//是病毒 返回真
			return TRUE;
		}
	}
	return FALSE;
}

//白名单
BOOL CViresScan::WhiteScan(WCHAR* wzPath)
{

	return TRUE;
}