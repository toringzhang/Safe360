#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <Shlobj.h>
#include <Shlwapi.h>
using namespace std;

//操作cookie包含的头文件
#include <wininet.h>
#include <UrlHist.h>
#pragma comment(lib,"Wininet.lib")

#pragma comment(lib,"Shlwapi.lib")
#define SWEEP_BUFFER_SIZE 100000
//清理项ID枚举
enum DEL_RUBBISH_TYPE
{
	CLEAN_SYSTEM_TEMP = 0,     //系统临时文件
	CLEAN_RUBBISH_STATION,     //垃圾回收站
	CLEAN_RUN_HISTORY,         //运行记录
	CLEAN_DOCUMENT_HISTORY,    //最近文档记录
	CLEAN_PREVUSER_HISTORY,    //用户登录记录
	CLEAN_FILEFIND_HISTORY,    //文件查找记录

	CLEAN_INTERNET_TEMP,       //Internet临时文件
	CLEAN_INTERNET_COOKIE,     //Internet Cookie
	CLEAN_ADDRBAR_HISTORY,     //地址栏历史记录
	CLEAN_PASSWORD_HISTORY,    //密码记录
	CLEAN_BROWSEADDR_HISTORY   //浏览网址记录
};

//清理的缓冲类型
enum DEL_CACHE_TYPE
{
	FILES = 0, COOKIES
};

//垃圾文件信息
typedef struct _RUBBISH_FILE_INFO
{
	WCHAR wzFilePath[MAX_PATH];
	WCHAR wzFileSize[0x10];
}RUBBISH_FILE_INFO, *PRUBBISH_FILE_INFO;

DWORD WINAPI CleanThread(LPVOID lpVoid);
class Rubbish
{
public:
	Rubbish();
	~Rubbish();
	BOOL StartCleanThread();

	VOID StartScan();

	VOID CleanRubbish(int CleanType);

	VOID CleanSystemTemp();

	BOOL EmptyDirectory(LPCTSTR szTempPath, BOOL bDeleteDesktopIni = FALSE, BOOL bWipeIndexDat=FALSE);

	BOOL ShowFileSize(DOUBLE dwSize, WCHAR * wzFileSize);

	BOOL WipeFile(LPCTSTR wzDirectory, LPCTSTR wzFilePath);

	VOID PrintRubbishInfo();

	VOID CleanRubbishStation();

	VOID CleanRunHistory();

	VOID CleanDocumentHistory();

	VOID CleanPrevUserHistory();

	VOID CleanFileFindHistory();

	VOID CleanInternetTemp();

	VOID CleanInternetCookie();

	VOID CleanAddressBarHistory();

	VOID CleanPasswordHistory();

	VOID CleanBrowseAddressHistory();

	BOOL DeleteUrlCache(DEL_CACHE_TYPE Type);

	
	BOOL m_IsScanFile;
private:
	vector<RUBBISH_FILE_INFO> m_RubbishFiles;
	double m_AllRubbishSize;
};