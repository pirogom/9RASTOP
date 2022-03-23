#pragma once

#define dMAX_REG_BUFSIZE		40960

class ConfigMgr
{
public:
	ConfigMgr();
	~ConfigMgr();

	DWORD		m_dwWinStarExe;
	DWORD		m_dwStartWithAllStop;
	DWORD		m_dwSiteAutoDetect;
	DWORD		m_dwWithInst;
	DWORD		m_dwHideAsRun;
	DWORD		m_dwDetectTime;
	TCHAR		m_szIgnore[dMAX_REG_BUFSIZE];

	void		LoadConfig() {
		m_dwWinStarExe = ReadDword(_T("RunStartup"));
		m_dwStartWithAllStop = ReadDword(_T("AllStopExec"));
		m_dwSiteAutoDetect = ReadDword(_T("AutoSiteDetect"));
		m_dwWithInst = ReadDword(_T("WithInst"));
		m_dwHideAsRun = ReadDword(_T("HideAsRun"));
		m_dwDetectTime = ReadDword(_T("DetectTime"));

		if (m_dwDetectTime >= 10)
			m_dwDetectTime = 0;

		ReadString(_T("IgnoreSite"), m_szIgnore);
	}

	void		SaveConfig() {
		WriteDword(_T("RunStartup"), m_dwWinStarExe);
		WriteDword(_T("AllStopExec"), m_dwStartWithAllStop);
		WriteDword(_T("AutoSiteDetect"), m_dwSiteAutoDetect);
		WriteDword(_T("WithInst"), m_dwWithInst);
		WriteDword(_T("HideAsRun"), m_dwHideAsRun);
		WriteDword(_T("DetectTime"), m_dwDetectTime);
		WriteString(_T("IgnoreSite"), m_szIgnore);
	}

	BOOL WriteString(TCHAR *key, TCHAR *value )
	{
		HKEY hSubKey = NULL;

		// open the key
		if (RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\ModuPlug"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSubKey, NULL) == ERROR_SUCCESS)
		{
			RegSetValueEx(hSubKey, key, NULL, REG_SZ, (BYTE*)value,(DWORD)wcslen(value)+1);

			// 키 닫기
			RegCloseKey(hSubKey);

			return TRUE;
		}

		return FALSE;
	}
	BOOL WriteDword(TCHAR *key, DWORD value)
	{
		HKEY hSubKey = NULL;

		if (RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\ModuPlug"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSubKey, NULL) == ERROR_SUCCESS)
		{
			RegSetValueEx(hSubKey, key, NULL, REG_DWORD, (LPBYTE)&value, sizeof(DWORD));

			// 키 닫기
			RegCloseKey(hSubKey);

			return TRUE;
		}

		return FALSE;
	}


	BOOL ReadString(TCHAR *key, TCHAR *retValue)
	{
		HKEY hSubKey = NULL;
		retValue[0] = NULL;

		// open the key
		if (RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\ModuPlug"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSubKey, NULL) == ERROR_SUCCESS)
		{
			DWORD buf_size = 0;

			// 문자열의 크기를 먼저 읽어온다.
			if (RegQueryValueEx(hSubKey, key, NULL, NULL, NULL, &buf_size) == ERROR_SUCCESS)
			{

				if (buf_size >= dMAX_REG_BUFSIZE) {
					AfxMessageBox(_T("레지스트리 버퍼 오류!!"));
					exit(1);
				}

				// 실제 값을 읽어온다.
				if (RegQueryValueEx(hSubKey, key, NULL, NULL, (LPBYTE)retValue, &buf_size) == ERROR_SUCCESS)
				{
					retValue[buf_size] = _T('\0');
					// 키 닫기
					RegCloseKey(hSubKey);
					return TRUE;
				}
				else {
					// 키 닫기
					RegCloseKey(hSubKey);
					return FALSE;
				}
			}
			else {
				// 키 닫기
				RegCloseKey(hSubKey);
				return FALSE;
			}

			// 키 닫기
			RegCloseKey(hSubKey);
			return TRUE;
		}

		return FALSE;
	}


	DWORD ReadDword(TCHAR *key)
	{
		DWORD dwValue = 0;
		HKEY hSubKey = NULL;

		// 키 열기
		if (RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\ModuPlug"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSubKey, NULL) == ERROR_SUCCESS)
		{
			DWORD buf_size = sizeof(DWORD);

			// DWORD 값 읽어오기
			if (RegQueryValueEx(hSubKey, key, NULL, NULL, (LPBYTE)&dwValue, &buf_size) != ERROR_SUCCESS)
			{
				dwValue = 0;
			}

			// 키 닫기
			RegCloseKey(hSubKey);
		}

		return dwValue;
	}



};

