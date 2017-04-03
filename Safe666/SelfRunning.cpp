#include "stdafx.h"
#include "SelfRunning.h"


SelfRunning::SelfRunning()
{
}


SelfRunning::~SelfRunning()
{
}

void SelfRunning::GetSelfRunningSoftware()
{
	SELFRUNNINGINFO SelfRunningInfo = { 0 };

	HKEY hkRootKey = NULL;						// 主键
	HKEY hkResult  = NULL;						// 将要打开键的句柄 
	LONG Return;						// 记录读取注册表是否成功
	DWORD Index = 0;
	TCHAR szKeyName[255] = { 0 };       // 注册表项名称
	DWORD dwKeyLen = 255;
	DWORD dwNameLen = 255;
	DWORD dwType = 0;

	LPCTSTR lpSubKey[2];				// 子键名称
	DWORD ListIndex = 0;

	lpSubKey[0] = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	lpSubKey[1] = _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run");
	
	for (DWORD j = 0; j < 2; j++)
	{
		if (j == 0)
			hkRootKey = HKEY_CURRENT_USER;
		if (j == 1)
			hkRootKey = HKEY_LOCAL_MACHINE;
		for (DWORD i = 0; i < 2; i++)
		{
			//打开lpSubkey
			Return = RegOpenKeyEx(hkRootKey, lpSubKey[i], 0,
				KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE, &hkResult);
			if (Return == ERROR_SUCCESS)
			{
				//如果打开成功，开始枚举其值
				while (ERROR_NO_MORE_ITEMS !=
					RegEnumValue(hkResult, Index, SelfRunningInfo.wzBootName, &dwKeyLen,
						0, NULL, NULL, NULL))
				{
					//获取每个值得数据 szKeyName为值得名称 buffer为值的数据
					RegQueryValueEx(hkResult, SelfRunningInfo.wzBootName,
						0, &dwType, (LPBYTE)SelfRunningInfo.wzBootOrder, &dwNameLen);

					//主键信息
					SelfRunningInfo.wzHKEY;
					if (j == 0)
						_tcscpy_s(SelfRunningInfo.wzHKEY, _T("HKEY_CURRENT_USER"));
					if (j == 1)
						_tcscpy_s(SelfRunningInfo.wzHKEY, _T("HKEY_LOCAL_MACHINE"));
					//删除时要用到的
					_tcscpy_s(SelfRunningInfo.wzBootPos, lpSubKey[i]);

					//添加到vector
					m_SelfRunningSoftware.push_back(SelfRunningInfo);
					
					Index++;
					dwNameLen = 255;
					dwKeyLen = 255;
					memset(&SelfRunningInfo, 0, sizeof(SELFRUNNINGINFO));
				}
				Index = 0;	
			}		
		}
		//关闭注册表
		RegCloseKey(hkResult);
	}
	
}


void SelfRunning::PrintInfo()
{
	printf("------------------自启动软件----------------------\r\n");
	vector<SELFRUNNINGINFO>::iterator SelfRunningSoftware;
	for (SelfRunningSoftware = m_SelfRunningSoftware.begin(); SelfRunningSoftware != m_SelfRunningSoftware.end(); ++SelfRunningSoftware)
	{
		printf("************************************************\r\n");
		printf("启动项：%S\r\n", SelfRunningSoftware->wzBootName);
		printf("启动命令：%S\r\n", SelfRunningSoftware->wzBootOrder);
		printf("注册表信息：%S\\%S\r\n", SelfRunningSoftware->wzHKEY, SelfRunningSoftware->wzBootPos);
		printf("************************************************\r\n");
	}
	printf("-------------------------------------------------\r\n");
}
