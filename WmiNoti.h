#ifndef __WMI_NOTI_H__
#define __WMI_NOTI_H__

#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

#include "EventSink.h"

class CWmiNoti {
private:
	IWbemLocator			*m_pLoc;
	IWbemServices			*m_pSvc;
	IUnsecuredApartment		*m_pUnsecApp;
	EventSink				*m_pSink;
	IUnknown				*m_pStubUnk;
	IWbemObjectSink			*m_pStubSink;
	BOOL					m_InitSuccess;
	HWND					m_hWnd;

public:
	CWmiNoti();
	~CWmiNoti();

	int		Init(HWND hWnd);
	void	Destroy();
};

#endif