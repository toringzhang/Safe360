#include "stdafx.h"
#include "Rubbish.h"


Rubbish::Rubbish()
{
	m_IsScanFile = TRUE;
	m_AllRubbishSize = (double)0;
}


Rubbish::~Rubbish()
{
}

BOOL Rubbish::StartCleanThread()
{
	CloseHandle(CreateThread(NULL, 0, CleanThread, this, 0, NULL));
	return TRUE;
}

VOID Rubbish::StartScan()
{
	m_IsScanFile = TRUE;
}

//扫描线程
DWORD WINAPI CleanThread(LPVOID lpVoid)
{
	Rubbish rs;
	for (DWORD i = 0; i < 11; i++)
	{
		rs.CleanRubbish(i);
	}
	return 0;
}

//启动清理
VOID Rubbish::CleanRubbish(int CleanType)
{
	switch (CleanType)
	{
	case CLEAN_SYSTEM_TEMP:
		CleanSystemTemp();
		break;
	case CLEAN_RUBBISH_STATION:
		CleanRubbishStation();
		break;
	case CLEAN_RUN_HISTORY:
		CleanRunHistory();
		break;
	case CLEAN_DOCUMENT_HISTORY:
		CleanDocumentHistory();
		break;
	case CLEAN_PREVUSER_HISTORY:
		CleanPrevUserHistory();
		break;
	case CLEAN_FILEFIND_HISTORY:
		CleanFileFindHistory();
		break;
	case CLEAN_INTERNET_TEMP:
		CleanInternetTemp();
		break;
	case CLEAN_INTERNET_COOKIE:
		CleanInternetCookie();
		break;
	case CLEAN_ADDRBAR_HISTORY:
		CleanAddressBarHistory();
		break;
	case CLEAN_PASSWORD_HISTORY:
		CleanPasswordHistory();
		break;
	case CLEAN_BROWSEADDR_HISTORY:
		CleanBrowseAddressHistory();
		break;
	}
}

//清理系统临时文件
VOID Rubbish::CleanSystemTemp()
{
	WCHAR TempPath[MAX_PATH];
	//该API函数检索临时文件目录路径
	BOOL bSuccess = GetTempPath(MAX_PATH, TempPath);

	//ZeroMemory(TempPath, sizeof(TempPath));

	if (bSuccess)//得到系统临时目录
	{
		EmptyDirectory(TempPath, TRUE);
	}
}

//清空临时文件夹
BOOL Rubbish::EmptyDirectory(LPCTSTR szTempPath,
	BOOL bDeleteDesktopIni /*= FALSE*/, BOOL bWipeIndexDat /*= FALSE*/)
{
	WIN32_FIND_DATA wfd;
	HANDLE FindFileHandle = NULL;

	DWORD dwAttributes = 0;
	//必须初始化，否则lstrcat函数会认为未初始化内容为字符串
	WCHAR wzFileFullPath[MAX_PATH] = {0};
	lstrcat(wzFileFullPath, (WCHAR*)szTempPath);
	lstrcat(wzFileFullPath, L"*.*");

	if ((FindFileHandle = FindFirstFile(wzFileFullPath, &wfd)) == INVALID_HANDLE_VALUE)
		return FALSE;
	do
	{
		if (_tcscmp(wfd.cFileName, TEXT(".")) == 0 ||
			_tcscmp(wfd.cFileName, TEXT("..")) == 0 ||
			(bDeleteDesktopIni == FALSE && _tcsicmp(wfd.cFileName, TEXT("desktop.ini")) == 0))
		{
			continue;
		}

		WCHAR wzFilePath[MAX_PATH] = {0};
		lstrcat(wzFilePath, (WCHAR*)szTempPath);
		lstrcat(wzFilePath, wfd.cFileName);

		RUBBISH_FILE_INFO RubbishFileInfo = {0};
		//判断是否是扫描文件，如果是 就输出到List
		if (m_IsScanFile)
		{
			//插入到List
			if (wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				//如果是目录，则递归调用，列举下级目录
				EmptyDirectory(wzFilePath);
				continue;
			}
			//文件路径
			lstrcat(RubbishFileInfo.wzFilePath, wzFilePath);
			//文件大小
			ShowFileSize(wfd.nFileSizeLow, RubbishFileInfo.wzFileSize);
			m_RubbishFiles.push_back(RubbishFileInfo);
			setlocale(LC_ALL, "");
			printf("文件路径：%S\r\n", RubbishFileInfo.wzFilePath);
			printf("文件大小：%S\r\n", RubbishFileInfo.wzFileSize);
			//保存所有文件的大小 用于显示有多少
			m_AllRubbishSize += wfd.nFileSizeLow;
			continue;
		}

		//去掉只读属性
		dwAttributes = GetFileAttributes(wzFilePath);
		if (dwAttributes & FILE_ATTRIBUTE_READONLY)
		{
			dwAttributes &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributes(wzFilePath, dwAttributes);
		}
		//递归文件夹中的文件
		if (wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			//如果文件大小等于0说明是文件夹 递归查询
			EmptyDirectory(wzFilePath);
			//删除完文件夹中的文件，再删除文件夹
			RemoveDirectory(wzFilePath);
		}
		else
		{
			if (bWipeIndexDat && _tcsicmp(wfd.cFileName, TEXT("index.dat")) == 0)
			{
				WipeFile(szTempPath, wfd.cFileName);
			}
			//保存所有文件的大小 用于显示有多少
			m_AllRubbishSize += wfd.nFileSizeLow;
			if (!DeleteFile(wzFilePath))
			{
				m_AllRubbishSize -= wfd.nFileSizeLow;
			}
		}
	} while (FindNextFile(FindFileHandle, &wfd));
	FindClose(FindFileHandle);
	//PrintRubbishInfo();
	return TRUE;
}


BOOL Rubbish::ShowFileSize(DOUBLE dwSize, WCHAR* wzFileSize)
{
	
	if (dwSize<1024)
	{
		//B
		swprintf(wzFileSize, L"%.2f B", dwSize);
		return TRUE;
	}
	else if (dwSize<1024 * 1024)
	{
		//KB
		swprintf(wzFileSize, L"%.2f KB", dwSize/1024.0f);
		return TRUE;
	}
	else if (dwSize<1024 * 1024 * 1024)
	{
		//MB
		swprintf(wzFileSize, L"%.2f MB", dwSize / 1024.0f/1024.0f);
		return TRUE;
	}
	else if (dwSize>1024 * 1024 * 1024)
	{
		//GB
		swprintf(wzFileSize, L"%.2f GB", dwSize / 1024.0f / 1024.0f/1024.0f);
		return TRUE;
	}
	return FALSE;
}

//擦除文件内容
BOOL Rubbish::WipeFile(LPCTSTR wzDirectory, LPCTSTR wzFilePath)
{

	HANDLE	FileHandle;
	DWORD	dwSize;
	DWORD	dwWrite;
	TCHAR	sZero[SWEEP_BUFFER_SIZE];
	memset(sZero, 0, SWEEP_BUFFER_SIZE);

	WCHAR* wzFileFullPath = new WCHAR(MAX_PATH);
	lstrcat(wzFileFullPath, (WCHAR*)wzDirectory);
	lstrcat(wzFileFullPath, L"\\");
	lstrcat(wzFileFullPath, (WCHAR*)wzFilePath);
	

	FileHandle = CreateFile(wzFileFullPath, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (FileHandle == INVALID_HANDLE_VALUE)
		return FALSE;

	dwSize = GetFileSize(FileHandle, NULL);

	//skip file header (actually, I don't know the file format of index.dat)
	dwSize -= 64;
	SetFilePointer(FileHandle, 64, NULL, FILE_BEGIN);

	while (dwSize > 0)
	{
		if (dwSize > SWEEP_BUFFER_SIZE)
		{
			WriteFile(FileHandle, sZero, SWEEP_BUFFER_SIZE, &dwWrite, NULL);
			dwSize -= SWEEP_BUFFER_SIZE;
		}
		else
		{
			WriteFile(FileHandle, sZero, dwSize, &dwWrite, NULL);
			break;
		}
	}

	CloseHandle(FileHandle);
	return TRUE;
}


VOID Rubbish::PrintRubbishInfo()
{
	
	printf("----------------------垃圾信息----------------------\r\n");
	WCHAR AllRubbishSize[0x20];
	ShowFileSize(m_AllRubbishSize, AllRubbishSize);
	printf("垃圾大小：%S\r\n", AllRubbishSize);
	vector<RUBBISH_FILE_INFO>::iterator RubbishInfo;
	for (RubbishInfo = m_RubbishFiles.begin(); RubbishInfo != m_RubbishFiles.end(); ++RubbishInfo)
	{
		setlocale(LC_ALL, "");
		printf("***************************\r\n");
		printf("文件路径：%S\r\n",RubbishInfo->wzFilePath);
		printf("文件大小：%S\r\n", RubbishInfo->wzFileSize);
		printf("***************************\r\n");
	}
	printf("----------------------------------------------------\r\n");
}

//清理回收站
VOID Rubbish::CleanRubbishStation()
{
	//如果是扫描模式，直接不处理
	if (m_IsScanFile)
	{
		return;
	}		
	//清空指定驱动器上的回收站，第一参数表示窗口句柄，如果第二参数为NULL，则清理所有回收站，
	//第三参数 显示确认删除对话框|不显示删除进度|操作完成后不会播放声音
	SHEmptyRecycleBin(NULL, NULL,
		SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
}

//清理运行记录
VOID Rubbish::CleanRunHistory()
{
	if (m_IsScanFile)
	{
		return;
	}
	//从注册表中删除所有子键及其后代	
	SHDeleteKey(HKEY_CURRENT_USER,
		TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU"));
}

//清理最近文档记录
VOID Rubbish::CleanDocumentHistory()
{
	WCHAR DocPath[MAX_PATH] = { 0 };

	//检索由CSIDL标示的特殊文件夹路径
	BOOL bOk = SHGetSpecialFolderPath(NULL,
		DocPath, CSIDL_RECENT, FALSE);
	
	if (bOk)
	{
		EmptyDirectory(DocPath);
	}
	if (m_IsScanFile)
	{
		return;
	}	
	SHDeleteKey(HKEY_CURRENT_USER,
		TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs"));
}

//清理上次用户登录记录
VOID Rubbish::CleanPrevUserHistory()
{
	if (m_IsScanFile)
		return;
	//删除注册表子键的值
	SHDeleteValue(HKEY_CURRENT_USER,
		TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"),
		TEXT("DefaultUserName"));
	SHDeleteValue(HKEY_CURRENT_USER,
		TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"),
		TEXT("AltDefaultUserName"));
	SHDeleteValue(HKEY_LOCAL_MACHINE,
		TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Winlogon"),
		TEXT("DefaultUserName"));
}

//清理文件查找记录
VOID Rubbish::CleanFileFindHistory()
{
	if (m_IsScanFile)
		return;
	SHDeleteKey(HKEY_CURRENT_USER,
		TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Doc Find Spec MRU"));
	SHDeleteKey(HKEY_CURRENT_USER,
		TEXT("Software\\Microsoft\\Internet Explorer\\Explorer Bars\\{C4EE31F3-4768-11D2-BE5C-00A0C9A83DA1}\\ContainingTextMRU"));
	SHDeleteKey(HKEY_CURRENT_USER,
		TEXT("Software\\Microsoft\\Internet Explorer\\Explorer Bars\\{C4EE31F3-4768-11D2-BE5C-00A0C9A83DA1}\\FilesNamedMRU"));
}

VOID Rubbish::CleanInternetTemp()
{
	WCHAR InternetPath[MAX_PATH] = { 0 };
	if (!m_IsScanFile)
	{
		DeleteUrlCache(FILES);
	}
	//SHGetSpecialFolderPath 获取系统指定系统路径
	BOOL bOk = SHGetSpecialFolderPath(NULL,
		InternetPath, CSIDL_INTERNET_CACHE, FALSE);

	if (bOk)
	{  //得到临时目录，并清空它.
		EmptyDirectory(InternetPath);
	}
}

//清理Internet Cookie
VOID Rubbish::CleanInternetCookie()
{
	WCHAR CookiePath[MAX_PATH] = { 0 };
	DeleteUrlCache(COOKIES);
	BOOL bOk = SHGetSpecialFolderPath(NULL,
		CookiePath, CSIDL_COOKIES, FALSE);
	
	if (bOk)
	{//得到目录，并清空
		EmptyDirectory(CookiePath);
	}
}

//清理地址栏历史记录
VOID Rubbish::CleanAddressBarHistory()
{
	if (m_IsScanFile)
		return;
	SHDeleteKey(HKEY_CURRENT_USER,
		TEXT("Software\\Microsoft\\Internet Explorer\\TypedURLs"));
}

//清理密码记录
VOID Rubbish::CleanPasswordHistory()
{
	if (m_IsScanFile)
		return;
	SHDeleteKey(HKEY_CURRENT_USER,
		TEXT("Software\\Microsoft\\Internet Explorer\\IntelliForms"));
}

//清理浏览网址记录
VOID Rubbish::CleanBrowseAddressHistory()
{
	if (m_IsScanFile)
		return;
	HRESULT HResult;//返回值
	WCHAR HistoryPath[MAX_PATH] = { 0 };
	IUrlHistoryStg2* pUrlHistoryStg2 = NULL;
	//创建选件类的对象与指定的选件类ID或程序ID.
	HResult = CoCreateInstance(CLSID_CUrlHistory, NULL,
		CLSCTX_INPROC_SERVER, IID_IUrlHistoryStg2,
		(VOID**)&pUrlHistoryStg2);
	if (SUCCEEDED(HResult))
	{
		HResult = pUrlHistoryStg2->ClearHistory();
		pUrlHistoryStg2->Release();
	}

	delete pUrlHistoryStg2;

	// 如果上面代码不能清
	// 则有下面的，不完美，但能工作.
	//这里得到System32文件夹
	GetWindowsDirectory(HistoryPath, MAX_PATH);
	lstrcat(HistoryPath,L"\\History");
	EmptyDirectory(HistoryPath, FALSE, TRUE);

	BOOL bSuccess = SHGetSpecialFolderPath(NULL, HistoryPath, CSIDL_HISTORY, FALSE);
	if (bSuccess)
	{
		EmptyDirectory(HistoryPath, FALSE, TRUE);
	}
}


//清除Url缓存
BOOL Rubbish::DeleteUrlCache(DEL_CACHE_TYPE Type)
{
	BOOL bRet = FALSE;   //返回值
	HANDLE EntryHandle;
	LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;
	DWORD dwEntrySize;

	//delete the files
	dwEntrySize = 0;
	//获得INTERNET_CACHE_ENTRY_INFO的大小,第一次调用传NULL会分配所需大小
	EntryHandle = FindFirstUrlCacheEntry(NULL, NULL, &dwEntrySize);
	//分配响应大小的缓冲区
	lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) malloc(dwEntrySize);
	//获得进入口的句柄
	EntryHandle = FindFirstUrlCacheEntry(NULL, lpCacheEntry, &dwEntrySize);

	if (EntryHandle)
	{
		do
		{
			if (Type == FILES &&
				!(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
			{
				DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
			}
			else if (Type == COOKIES &&
				(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))
			{
				DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
			}
			dwEntrySize = 0;
			FindNextUrlCacheEntry(EntryHandle, NULL, &dwEntrySize);
			free(lpCacheEntry);
			lpCacheEntry = NULL;
			lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO)malloc(dwEntrySize);
		} while (FindNextUrlCacheEntry(EntryHandle, lpCacheEntry, &dwEntrySize));

		bRet = TRUE;
	}

	if (lpCacheEntry)
	{
		free(lpCacheEntry);
		lpCacheEntry = NULL;
	}
	return bRet;
}