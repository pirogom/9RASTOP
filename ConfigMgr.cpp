#include "stdafx.h"
#include "ConfigMgr.h"


ConfigMgr::ConfigMgr()
{
	m_dwWinStarExe = 0;
	m_dwStartWithAllStop = 0;
	m_dwSiteAutoDetect = 0;
	m_dwWithInst = 0;
	m_dwHideAsRun = 0;
	*m_szIgnore = NULL;
}


ConfigMgr::~ConfigMgr()
{
}
