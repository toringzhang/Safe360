#include "stdafx.h"
#include "SoftwareManager.h"


SoftwareManager::SoftwareManager()
{
}


SoftwareManager::~SoftwareManager()
{
}

//通过读取注册表获取软件信息
void SoftwareManager::GetSoftwareInfo()
{
	SOFTWAREINFO SoftwareInfo = {0};
	HKEY hkRootKey;          // 主键
	LPCTSTR lpSubKey[2];        // 子键名称
	HKEY hkResult;           // 将要打开键的句柄 
	HKEY hkRKey;
	LONG Return;             // 记录读取注册表是否成功
	TCHAR wzKeyName[255] = { 0 };        // 注册表项名称
	
	hkRootKey = HKEY_LOCAL_MACHINE;
	//32位EXE
	lpSubKey[0] = Win32PATH;
	//64位EXE
	lpSubKey[1] = Win64PATH;
	printf("------------------软件信息-----------------\r\n");
	for (int i = 0; i < 2; i++)
	{
		//打开注册表
		Return = RegOpenKeyEx(hkRootKey, lpSubKey[i], 0,
			KEY_ENUMERATE_SUB_KEYS, &hkResult);

		WCHAR wzBuffer[255] = { 0 };

		DWORD dwIndex = 0;
		if (Return == ERROR_SUCCESS)
		{

			DWORD dwListIndex = 0;
			DWORD dwKeyLength = 255;
			
			//要想使用RegEnumKeyEx则至少有KEY_ENUMERATE_SUB_KEYS权限才行
			while (ERROR_NO_MORE_ITEMS != RegEnumKeyEx(hkResult, dwIndex, wzKeyName, &dwKeyLength, NULL, NULL, NULL, NULL))
			{
				if (wzKeyName != L"")
				{
					lstrcat(wzBuffer, (LPWSTR)lpSubKey);
					lstrcat(wzBuffer, L"\\");
					lstrcat(wzBuffer, wzKeyName);
					DWORD dwType = 0;
					DWORD dwLength = 255;
					if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, wzBuffer,
						NULL, KEY_QUERY_VALUE, &hkRKey) == ERROR_SUCCESS)
					{

						DWORD Ret = RegQueryValueEx(hkRKey, _T("DisplayName"),
							NULL, &dwType, (LPBYTE)SoftwareInfo.wzSoftName, &dwLength);
						dwLength = 255;

						RegQueryValueEx(hkRKey, _T("DisplayVersion"),
							NULL, &dwType, (LPBYTE)SoftwareInfo.wzSoftVersion, &dwLength);
						dwLength = 255;

						RegQueryValueEx(hkRKey, _T("InstallLocation"),
							NULL, &dwType, (LPBYTE)SoftwareInfo.wzSoftInstallPath, &dwLength);
						dwLength = 255;

						RegQueryValueEx(hkRKey, _T("Publisher"),
							NULL, &dwType, (LPBYTE)SoftwareInfo.wzSoftPublisher, &dwLength);
						dwLength = 255;

						RegQueryValueEx(hkRKey, _T("UninstallString"),
							NULL, &dwType, (LPBYTE)SoftwareInfo.wzSoftUninstallPath, &dwLength);
						dwLength = 255;

						RegQueryValueEx(hkRKey, _T("InstallDate"),
							NULL, &dwType, (LPBYTE)SoftwareInfo.wzSoftDate, &dwLength);
						if (SoftwareInfo.wzSoftDate[0])
						{
							SoftwareInfo.wzSoftDate[9] = SoftwareInfo.wzSoftDate[7];
							SoftwareInfo.wzSoftDate[8] = SoftwareInfo.wzSoftDate[6];
							SoftwareInfo.wzSoftDate[7] = '-';
							SoftwareInfo.wzSoftDate[6] = SoftwareInfo.wzSoftDate[5];
							SoftwareInfo.wzSoftDate[5] = SoftwareInfo.wzSoftDate[4];
							SoftwareInfo.wzSoftDate[4] = '-';
						}
						dwLength = 255;

						//保存数据
						if (SoftwareInfo.wzSoftName[0] != '\0')
						{
							m_SoftwareInfo.push_back(SoftwareInfo);

						}

						dwIndex++;
					}
					dwKeyLength = 255;
					memset(wzKeyName, 0, 255);
					memset(wzBuffer, 0, 255);
					memset(&SoftwareInfo, 0, sizeof(SOFTWAREINFO));
				}
			}
			RegCloseKey(hkResult);
			
		}
	}
	printf("-------------------------------------------\r\n");
}

BOOL SoftwareManager::IsX64()
{
	SYSTEM_INFO SystemInfo;
	GetNativeSystemInfo(&SystemInfo);
	if (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		SystemInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_IA64)
	{
		//64 位操作系统
		return TRUE;
	}
	else
	{
		// 32 位操作系统
		return FALSE;
	}
	return FALSE;
}

BOOL SoftwareManager::PrintSoftwareInfo()
{
	vector<SOFTWAREINFO>::iterator SoftwareInfo;
	for (SoftwareInfo=m_SoftwareInfo.begin();SoftwareInfo!=m_SoftwareInfo.end();++SoftwareInfo)
	{
		printf("软件名称:%S \r\n", SoftwareInfo->wzSoftName);
		printf("软件版本号:%S \r\n", SoftwareInfo->wzSoftVersion);
		printf("软件发布厂商:%S \r\n", SoftwareInfo->wzSoftPublisher);
		printf("软件安装日期:%S \r\n", SoftwareInfo->wzSoftDate);
		printf("软件安装路径:%S \r\n", SoftwareInfo->wzSoftInstallPath);
		printf("软件卸载路径:%S \r\n", SoftwareInfo->wzSoftUninstallPath);
	}
	return TRUE;
}
