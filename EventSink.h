// EventSink.h
#ifndef EVENTSINK_H
#define EVENTSINK_H

class EventSink : public IWbemObjectSink
{
	LONG m_lRef;
	bool bDone;

public:
	HWND			m_hWnd;
	EventSink() { m_lRef = 0; }
	~EventSink() { bDone = true; }

	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT
		STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

	virtual HRESULT STDMETHODCALLTYPE Indicate(
		LONG lObjectCount,
		IWbemClassObject __RPC_FAR* __RPC_FAR* apObjArray
	);

	virtual HRESULT STDMETHODCALLTYPE SetStatus(
		/* [in] */ LONG lFlags,
		/* [in] */ HRESULT hResult,
		/* [in] */ BSTR strParam,
		/* [in] */ IWbemClassObject __RPC_FAR* pObjParam
	);


};

#endif    // end of EventSink.h