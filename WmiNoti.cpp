#include "stdafx.h"
#include "WmiNoti.h"

CWmiNoti::CWmiNoti() {
	m_pLoc = NULL;
	m_pSvc = NULL;
	m_pUnsecApp = NULL;
	m_pSink = NULL;
	m_pStubUnk = NULL;
	m_pStubSink = NULL;
	m_InitSuccess = FALSE;
	m_hWnd = NULL;
}

CWmiNoti::~CWmiNoti() {

}


int		CWmiNoti::Init(HWND hWnd) {
	m_hWnd = hWnd;
	m_InitSuccess = FALSE;

	HRESULT hres;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		return 1;                  // Program has failed.
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------

	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM negotiates service
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);


	if (FAILED(hres))
	{
		CoUninitialize();
		return 1;                      // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------
	m_pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)& m_pLoc);

	if (FAILED(hres))
	{
		CoUninitialize();
		return 1;                 // Program has failed.
	}

	// Step 4: ---------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	m_pSvc = NULL;

	// Connect to the local root\cimv2 namespace
	// and obtain pointer pSvc to make IWbemServices calls.
	hres = m_pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&m_pSvc
	);

	if (FAILED(hres))
	{
		m_pLoc->Release();
		CoUninitialize();
		return 1;                // Program has failed.
	}


	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------
	hres = CoSetProxyBlanket(
		m_pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx 
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx 
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		m_pSvc->Release();
		m_pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}

	// Step 6: -------------------------------------------------
	// Receive event notifications -----------------------------

	// Use an unsecured apartment for security
	m_pUnsecApp = NULL;

	hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL,
		CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment,
		(void**)& m_pUnsecApp);

	m_pSink = new EventSink;
	m_pSink->AddRef();
	m_pSink->m_hWnd = hWnd;

	m_pStubUnk = NULL;
	m_pUnsecApp->CreateObjectStub(m_pSink, &m_pStubUnk);

	m_pStubSink = NULL;
	m_pStubUnk->QueryInterface(IID_IWbemObjectSink,
		(void**)&m_pStubSink);

	// The ExecNotificationQueryAsync method will call
	// The EventQuery::Indicate method when an event occurs
	/*
	chrome.exe
	MicrosoftEdgeCP.exe
	MicrosoftEdgeSH.exe
	MicrosoftEdge.exe
	firefox.exe
	*/
	hres = m_pSvc->ExecNotificationQueryAsync(
		_bstr_t("WQL"),
#ifdef USE_PROCESS_DETECT
		_bstr_t("SELECT * "
			"FROM __InstanceCreationEvent WITHIN 1 "
			"WHERE TargetInstance ISA 'Win32_Process'"),
#else
		_bstr_t("SELECT * "
			"FROM __InstanceCreationEvent WITHIN 1 "
			"WHERE TargetInstance ISA 'Win32_Process' AND ( "
		    "TargetInstance.Name ='chrome.exe' OR "
			"TargetInstance.Name ='MicrosoftEdgeCP.exe' OR "
			"TargetInstance.Name ='firefox.exe' OR "
			"TargetInstance.Name ='iexplore.exe'"
			" )" ),
#endif
		WBEM_FLAG_SEND_STATUS,
		NULL,
		m_pStubSink);

	// Check for errors.
	if (FAILED(hres))
	{
		m_pSvc->Release();
		m_pLoc->Release();
		m_pUnsecApp->Release();
		m_pStubUnk->Release();
		m_pSink->Release();
		m_pStubSink->Release();
		CoUninitialize();
		return 1;
	}

	// Wait for the event
	m_InitSuccess = TRUE;
	return 0;   // Program successfully completed.
}

void	CWmiNoti::Destroy() {
	if (m_InitSuccess == TRUE) {
		m_pSvc->CancelAsyncCall(m_pStubSink);

		// Cleanup
		// ========
		m_pSvc->Release();
		m_pLoc->Release();
		m_pUnsecApp->Release();
		m_pStubUnk->Release();
		m_pSink->Release();
		m_pStubSink->Release();
		CoUninitialize();
		m_InitSuccess = FALSE;
	}
	m_pLoc = NULL;
	m_pSvc = NULL;
	m_pUnsecApp = NULL;
	m_pSink = NULL;
	m_pStubUnk = NULL;
	m_pStubSink = NULL;
	m_InitSuccess = FALSE;
	m_hWnd = NULL;
}