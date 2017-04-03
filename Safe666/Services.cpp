#include "stdafx.h"
#include "Services.h"


Services::Services()
{
}


Services::~Services()
{
}


void Services::GetServicesInfo()
{
	//1.打开远程计算机服务控制管理器
	SC_HANDLE SCHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	//2.第一次调用，获取需要的内存大小
	DWORD dwServiceNum = 0;
	DWORD dwSize = 0;
	EnumServicesStatusEx(
		SCHandle,
		SC_ENUM_PROCESS_INFO,
		SERVICE_WIN32,
		SERVICE_STATE_ALL,	//所有服务状态
		NULL,				//缓冲区
		0,					//缓冲区大小
		&dwSize,			//需要的大小
		&dwServiceNum,		//缓冲区中的服务个数
		NULL,
		NULL);
	//3.申请需要的内存，第二次调用
	LPENUM_SERVICE_STATUS_PROCESS lpEnumService =
		(LPENUM_SERVICE_STATUS_PROCESS)LocalAlloc(LPTR, dwSize);
	//4.第二次枚举
	EnumServicesStatusEx(
		SCHandle,
		SC_ENUM_PROCESS_INFO,
		SERVICE_WIN32,
		SERVICE_STATE_ALL,	//所有服务状态
		(LPBYTE)lpEnumService,		//缓冲区
		dwSize,				//缓冲区大小
		&dwSize,			//需要的大小
		&dwServiceNum,		//缓冲区中的服务个数
		NULL,
		NULL);
	//5.遍历信息
	SC_HANDLE ServiceHandle = nullptr;
	for (DWORD i = 0; i < dwServiceNum; i++)
	{
		//获取基础信息
		//1.服务名称
		//2.服务描述
		//3.服务状态(根据得到的值手动输出字符串)
		//“已停止” “正在运行" "正在暂停"...
		SERVICE_STATUS_C ServiceStatus = {0};
		//获取更多信息
		//1.打开服务
		ServiceHandle = OpenService(SCHandle,
			lpEnumService[i].lpServiceName, SERVICE_QUERY_CONFIG);
		// 2.第一次调用获取需要的缓冲区大小
		QueryServiceConfig(ServiceHandle, NULL, 0, &dwSize);
		//分配内存
		LPQUERY_SERVICE_CONFIG pServiceConfig =
			(LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, dwSize);
		//3.第二次调用,获取信息
		QueryServiceConfig(ServiceHandle, pServiceConfig, dwSize, &dwSize);
		//更新list中该服务的状态信息
		switch (lpEnumService[i].ServiceStatusProcess.dwCurrentState)
		{
		case SERVICE_STOPPED:
			lstrcpy(ServiceStatus.wzCurrentState, L"已停止");
			break;
		case SERVICE_STOP_PENDING:
			lstrcpy(ServiceStatus.wzCurrentState, L"正在停止");
			break;
		case SERVICE_START_PENDING:
			lstrcpy(ServiceStatus.wzCurrentState, L"正在启动");
			break;
		case SERVICE_RUNNING:
			lstrcpy(ServiceStatus.wzCurrentState, L"正在运行");
			break;
		case SERVICE_PAUSED:
			lstrcpy(ServiceStatus.wzCurrentState, L"已暂停");
			break;
		case SERVICE_PAUSE_PENDING:
			lstrcpy(ServiceStatus.wzCurrentState, L"正在暂停");
			break;
		case SERVICE_CONTINUE_PENDING:
			lstrcpy(ServiceStatus.wzCurrentState, L"准备继续");
			break;
		}

		switch (pServiceConfig->dwStartType)
		{
		case SERVICE_AUTO_START:
			lstrcpy(ServiceStatus.wzStartType, L"自动");
			break;
		case SERVICE_BOOT_START:
			lstrcpy(ServiceStatus.wzStartType, L"SERVICE_BOOT_START");
			break;
		case SERVICE_DEMAND_START:
			lstrcpy(ServiceStatus.wzStartType, L"手动");
			break;
		case SERVICE_DISABLED:
			lstrcpy(ServiceStatus.wzStartType, L"禁用");
			break;
		case SERVICE_SYSTEM_START:
			lstrcpy(ServiceStatus.wzStartType, L"SERVICE_SYSTEM_START");
			break;
		}
		switch (lpEnumService[i].ServiceStatusProcess.dwServiceType)
		{
		case SERVICE_FILE_SYSTEM_DRIVER:
			lstrcpy(ServiceStatus.wzServiceType, L"文件系统驱动服务");
			break;
		case SERVICE_KERNEL_DRIVER:
			lstrcpy(ServiceStatus.wzServiceType, L"驱动服务");
			break;
		case SERVICE_WIN32_OWN_PROCESS:
			lstrcpy(ServiceStatus.wzServiceType, L"独立进程服务");
			break;
		case SERVICE_WIN32_SHARE_PROCESS:
			lstrcpy(ServiceStatus.wzServiceType, L"共享进程服务");
			break;
		}
		ServiceStatus.lpServiceName  = lpEnumService[i].lpServiceName;
		ServiceStatus.lpDisplayName  = lpEnumService[i].lpDisplayName;
		m_ServicesInfo.push_back(ServiceStatus);
	}
	LocalFree(lpEnumService);
	CloseServiceHandle(SCHandle);
	CloseServiceHandle(ServiceHandle);
}

void Services::PrintServicesInfo()
{
	printf("----------------------系统服务----------------------\r\n");
	vector<SERVICE_STATUS_C>::iterator ServiceInfo;
	for (ServiceInfo = m_ServicesInfo.begin(); ServiceInfo != m_ServicesInfo.end(); ++ServiceInfo)
	{
		//打印中文字符串必须加这句话
		setlocale(LC_ALL, "");
		printf("********************************\r\n");
		printf("名称：%S\r\n", ServiceInfo->lpServiceName);
		printf("描述：%S\r\n", ServiceInfo->lpDisplayName);
		printf("状态：%S\r\n", ServiceInfo->wzCurrentState);
		printf("启动类型：%S\r\n", ServiceInfo->wzStartType);
		printf("服务类型：%S\r\n", ServiceInfo->wzServiceType);
		printf("********************************\r\n");
	}
	printf("---------------------------------------------------\r\n");
}