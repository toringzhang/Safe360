#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
using namespace std;

typedef struct _SERVICE_STATUS_C {
	LPWSTR  lpServiceName;
	LPWSTR  lpDisplayName;
	WCHAR   wzStartType[21];
	WCHAR   wzServiceType[10];
	WCHAR   wzCurrentState[5];
} SERVICE_STATUS_C, *LPSERVICE_STATUS_C;

class Services
{
public:
	Services();
	~Services();
	void GetServicesInfo();
	void PrintServicesInfo();
private:
	vector<SERVICE_STATUS_C> m_ServicesInfo;
};

