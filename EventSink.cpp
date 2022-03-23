// EventSink.cpp
#include "stdafx.h"

#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#include "EventSink.h"

#pragma comment(lib, "wbemuuid.lib")

ULONG EventSink::AddRef()
{
	return InterlockedIncrement(&m_lRef);
}

ULONG EventSink::Release()
{
	LONG lRef = InterlockedDecrement(&m_lRef);
	if (lRef == 0)
		delete this;
	return lRef;
}

HRESULT EventSink::QueryInterface(REFIID riid, void** ppv)
{
	if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
	{
		*ppv = (IWbemObjectSink*)this;
		AddRef();
		return WBEM_S_NO_ERROR;
	}
	else return E_NOINTERFACE;
}

HRESULT EventSink::Indicate(long lObjectCount, IWbemClassObject** apObjArray)
{
	HRESULT hr = S_OK;

	if (m_hWnd != NULL) {
		SendMessage(m_hWnd, WM_NEW_BROWSER, NULL, NULL);
	}


#ifdef USE_PROCESS_DETECT
		_variant_t vtProp;

		for (int i = 0; i < lObjectCount; i++)
		{
			hr = apObjArray[i]->Get(L"TargetInstance", 0, &vtProp, 0, 0);

			if (!FAILED(hr))
			{
				IUnknown *str = vtProp;
				IWbemClassObject *pObj = NULL;
				hr = str->QueryInterface(IID_IWbemClassObject, reinterpret_cast<void**>(&pObj));
				if (SUCCEEDED(hr))
				{
					_variant_t vtName, vtPid, vtPath, vtCommand;
					HRESULT nhr = pObj->Get(L"Name", 0, &vtName, NULL, NULL);
					HRESULT phr = pObj->Get(L"ProcessID", 0, &vtPid, NULL, NULL);
					HRESULT epr = pObj->Get(L"ExecutablePath", 0, &vtPath, NULL, NULL);
					HRESULT clr = pObj->Get(L"CommandLine", 0, &vtCommand, NULL, NULL);

					if (SUCCEEDED(nhr) && SUCCEEDED(phr) && SUCCEEDED(epr) && SUCCEEDED(clr))
					{
						DWORD dwPid = vtPid.intVal;
						CString szPName = (LPCTSTR)(bstr_t)vtName.bstrVal;
						CString szPath = (LPCTSTR)(bstr_t)vtPath.bstrVal;
						CString szCmd = (LPCTSTR)(bstr_t)vtCommand.bstrVal;


						CString msg;

						msg.Format(_T("Name : %s / PricessID : %d / Path : %s / CMD : %s\r\n"), szPName, dwPid, szPath, szCmd);

						OutputDebugString(msg);

					}
					VariantClear(&vtName);
					VariantClear(&vtPid);
				}
			}
			VariantClear(&vtProp);
		}
#endif

	return WBEM_S_NO_ERROR;
}

HRESULT EventSink::SetStatus(
	/* [in] */ LONG lFlags,
	/* [in] */ HRESULT hResult,
	/* [in] */ BSTR strParam,
	/* [in] */ IWbemClassObject __RPC_FAR* pObjParam
)
{
	if (lFlags == WBEM_STATUS_COMPLETE)
	{
	}
	else if (lFlags == WBEM_STATUS_PROGRESS)
	{
	}

	return WBEM_S_NO_ERROR;
}    // end of EventSink.cpp