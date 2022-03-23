
// 9RASTOPDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "9RASTOP.h"
#include "9RASTOPDlg.h"
#include "afxdialogex.h"

#include <shlobj.h>
#include <atlbase.h>              // 헤더파일 추가
#include <TlHelp32.h>

#include <winsvc.h>

#include "ConfigMgr.h"


#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define dPROG_NAME			"모두의플러그"

#define dSITE_DETECT_TIMER		10102
#define dUPDATE_TIMER_ID		10101
#define dDLG_HIDE_TIMER			10103

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Advapi32.lib")

typedef struct {
	TCHAR		*m_szCmdName;		// 실행파일
	TCHAR		*m_szSvcName;		// 서비스명 
	TCHAR		*m_szStartDir;		// 
} sONE_PRC_INFO, *sPONE_PRC_INFO;

typedef struct {
	TCHAR		*m_szCmdName;			// 실행파일(프로세스 리스트 체크용)
	TCHAR		*m_szInstName;			// 인스톨러
	TCHAR		*m_szInstParam;			// 인스톨 파라미터
	TCHAR		*m_szUninstallDir;
	TCHAR		*m_szUninstaller;
	TCHAR		*m_szUninstParam;
	TCHAR		*m_szRealUninstaller;
} sONE_INST_INFO, *sPONE_INST_INFO;

typedef struct {
	TCHAR								*m_szSiteName;				// 사이트명
	sONE_INST_INFO						m_InstInfo[12];				// 12개
	sONE_PRC_INFO						m_PrcInfo[32];				// 32개 
} sSITE_INFO, *sPSITE_INFO;

#define REGIST_SVC( cmdName, svcName ) { _T(cmdName), _T(svcName), NULL }
#define REGIST_PRG( cmdName, sdir ) { _T(cmdName), NULL, _T(sdir) }
#define REGIST_PRGN( cmdName ) { _T(cmdName), NULL, NULL }
#define REGIST_END() {NULL, NULL, NULL }

#define REGIST_INST( cmdName, instName, instParam, uninstDir, uninst, param, real ) {_T(cmdName),_T(instName), _T(instParam), _T(uninstDir), _T(uninst), _T(param), _T(real) }
#define REGIST_INST_END() {NULL,NULL,NULL, NULL,NULL,NULL, NULL}

#define SITE_END() { NULL, {{NULL,NULL,NULL, NULL,NULL,NULL,NULL}}, { {NULL,NULL, NULL } } }
#define NULL_STR	'\0'

/*
	{ "이름",
		{
			REGIST_INST_END(),
		},
		{
			REGIST_END(),
		}
	},
*/
const sSITE_INFO		g_sSiteInfo[] = {

	{ _T("정부민원포털 민원24"), 
		{
			REGIST_INST_END(),
		},
		{
			REGIST_SVC(	"G4CWSLocalServer.exe",		"G4CSSWSCSVC"),
			REGIST_SVC(	"AnySign4PCLauncher.exe",	"AnySign4PC Launcher"),
			REGIST_PRG( "CrossEXService.exe",		"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"), 
			REGIST_PRG( "AnySign4PC.exe",			"C:\\Program Files (x86)\\SoftForum\\XecureWeb\\AnySign\\dll"), 
			REGIST_END(),
		}
	},
	{ _T("정부24"),
		{
			REGIST_INST_END(),
		},
		{
			REGIST_SVC( "AnySign4PCLauncher.exe",		"AnySign4PC Launcher" ),
			REGIST_SVC( "G4CWSLocalServer.exe",			"G4CSSWSCSVC" ),
			REGIST_SVC( "ezPDFWSLauncher.exe",			"ezPDFWSLauncherG4C" ),
			REGIST_PRG( "veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20" ),
			REGIST_PRG(	"AnySign4PC.exe",				"C:\\Program Files (x86)\\SoftForum\\XecureWeb\\AnySign\\dll" ),
			REGIST_PRG( "CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex" ),
			REGIST_END(),
		}
	},

	{_T("대법원 전자가족관계등록시스템"),
		{
			// Ahnlab Safe Transcation
			//REGIST_SVC( "ASDSvc.exe",					"SafeTransactionSVC" ),
			//REGIST_PRG( "StSess.exe",					"C:\\Program Files\\AhnLab\\Safe Transaction"),
			//REGIST_PRG( "StSess32.exe",				"C:\\Program Files\\AhnLab\\Safe Transaction\\Nz32" ),
			//REGIST_INST( "ASDSvc.exe", "astx_setup.exe", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall" ),
			REGIST_INST( "StSess.exe", "astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST( "StSess32.exe", "astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC( "wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC( "AnySign4PCLauncher.exe",		"AnySign4PC Launcher"),
			REGIST_SVC( "IniClientSvc_x64.exe",			"INISAFEClientManager"),
			REGIST_SVC( "IniClientSvc.exe",				"INISAFEClientManager"),
			REGIST_SVC( "IMGSF50Svc.exe",				"IMGSF50_Svc" ),
			REGIST_PRG( "veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG( "AnySign4PC.exe",				"C:\\Program Files (x86)\\SoftForum\\XecureWeb\\AnySign\\dll"),
			REGIST_PRG( "INISAFECrossWebEXSvc.exe",		NULL_STR ),
			REGIST_PRG( "INISAFETrayEX.exe",			NULL_STR),
			REGIST_PRG( "IMGSF50Start_x64.exe",			NULL_STR),
			REGIST_PRG( "IMGSF50Start_x86.exe",			NULL_STR),
			REGIST_PRG( "MaEPSBroker.exe",				"C:\\Program Files (x86)\\MarkAny\\EPSBroker" ),
			REGIST_PRG( "WSActiveBridgeES.exe",			"C:\\Program Files (x86)\\VOICEYE\\WSActiveBridgeES" ),

			REGIST_END(),
		}
	},
	{_T("인터넷등기소"),
		{
			//nProtect
			//REGIST_SVC("nossvc.exe",							"nossvc"),
			//REGIST_PRG("nosstarter.npe",						NULL),
			REGIST_INST("nossvc.exe", "nos_setup.exe", "/S", "C:\\Program Files (x86)\\INCAInternet UnInstall\\nProtect Online Security", "nProtectUninstaller.exe", NULL_STR, "nProtectUninstaller.exe"),
			REGIST_INST_END(),
		},
		{
			REGIST_SVC( "AnySign4PCLauncher.exe",				"AnySign4PC Launcher"),
			REGIST_SVC( "macourtsafersvc.exe",					"MACOURTSAFER_Svc" ),
			REGIST_PRG( "AnySign4PC.exe",						"C:\\Program Files (x86)\\SoftForum\\XecureWeb\\AnySign\\dll" ),
			REGIST_PRG( "macourtsafer.exe",						NULL_STR ),
			REGIST_PRG( "CrossEXService.exe",					"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex" ),
			REGIST_PRG( "XecureHSM_Monitor.exe",				"C:\\Program Files (x86)\\SoftForum\\XecureHSM" ),

			REGIST_END(),
		},
	},

	{ _T("국세청 홈택스"),
		{
			REGIST_INST_END(),
		},
		{
			REGIST_SVC( "I3GMainSvc.exe",				"Interezen_service"),
			REGIST_SVC( "I3GMainSvc.exe",				"I3GMainSvc"),
			REGIST_SVC( "ObCrossEXService.exe",			"CrossEX Live Checker"),
			REGIST_SVC( "IMGSF50Svc.exe",				"IMGSF50_Svc"),
			REGIST_SVC( "raonk.svc.exe",				"RAON K" ),
			REGIST_SVC( "RexpertPrintService.exe",		"Rexpert30PrintService" ),
			REGIST_SVC( "wpmsvc.exe",					"WizveraPMSvc" ),
			REGIST_PRG( "CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex" ),
			REGIST_PRG( "IMGSF50Start_x64.exe",			NULL_STR ),
			REGIST_PRG( "IMGSF50Start_x86.exe",			NULL_STR ),
			REGIST_PRG( "raonk.moni.exe",				"C:\\ProgramData\\RAONWIZ\\RAON K"),
			REGIST_PRG( "raonk.exe",					NULL_STR ),
			REGIST_PRG( "veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20" ),
			REGIST_PRG( "I3GProc.exe",					"C:\\Program Files (x86)\\IPinside_LWS" ),
			REGIST_PRG( "MagicLine4NX.exe",				"C:\\Program Files (x86)\\DreamSecurity\\MagicLine4NX" ),
			REGIST_PRG( "NTSMagicXMLSecurityNP.exe",	"C:\\Program Files (x86)\\DreamSecurity\\NTSMagicXMLSecurityNP" ),
			REGIST_PRG( "RexpertPrintStartup.exe",		"C:\\Program Files (x86)\\clipsoft\\Rexpert30PrintService" ),
			REGIST_PRG( "rexpert.iojs.exe",				NULL_STR ),

			REGIST_END(),
		}
	},

	{ _T("Wetax 위택스"),
		{
			//REGIST_SVC("nossvc.exe",							"nossvc"),
			//REGIST_PRG("nosstarter.npe",						NULL),
			REGIST_INST("nossvc.exe", "nos_setup.exe", "/S", "C:\\Program Files (x86)\\INCAInternet UnInstall\\nProtect Online Security", "nProtectUninstaller.exe", NULL_STR, "nProtectUninstaller.exe"),
			REGIST_INST_END(),
		},
		{

			REGIST_SVC( "wpmsvc.exe",							"WizveraPMSvc" ),
			REGIST_SVC( "TPLSvc.exe",							"TPLSvc"),

			REGIST_PRG( "keysharpnxbiz.exe",					"C:\\Program Files (x86)\\KeySharpNxBiz"),
		
			REGIST_END(),
		}
	},

	{ _T("국민건강보험공단 - 사이버민원센터"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe				2396	Console
			REGIST_INST( "StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST( "StSess32.exe",	"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC(	"ObCrossEXService.exe",				"CrossEX Live Checker"),
			REGIST_SVC(	"AnySign4PCLauncher.exe",			"AnySign4PC Launcher"),
			REGIST_SVC( "wpmsvc.exe",						"WizveraPMSvc"),

			REGIST_PRG( "veraport.exe",						"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG( "AnySign4PC.exe",					"C:\\Program Files (x86)\\SoftForum\\XecureWeb\\AnySign\\dll"),
			REGIST_PRG( "CrossEXService.exe",				"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),

			REGIST_END(),
		}
	},
	{ _T("4대사회보험 정보연계센터"),
		{
			//	MaWebDRMSVC.exe				2888	Services
			REGIST_INST( "MaWebDRMSVC.exe",		"Inst_MaWebDRM.exe", "/S", "C:\\Program Files (x86)\\MarkAny\\WebDRMNoAX", "Uninst_WebDRM.exe", NULL_STR, "Un_A.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("IMGSF50Svc.exe",				"IMGSF50_Svc"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("EKT_WebSocket_Server.exe",		"C:\\Program Files (x86)\\SOFTELEVEN\\Easykeytec v2.3"),
			REGIST_PRG("IMGSF50Start_x64.exe",			NULL_STR),
			REGIST_PRG("IMGSF50Start_x86.exe",			NULL_STR),
			REGIST_PRG("MaWebDRMAgent.exe",				NULL_STR),
			REGIST_PRG("MaWebDRMAgent_x64.exe", 		NULL_STR),
			REGIST_PRG("soft11kbd.exe",					NULL_STR),
			REGIST_PRG("EZKeytecRun.exe",				NULL_STR),

			REGIST_END(),
		}
	},
	{ _T("특허청 특허로"),
		{
			//REGIST_SVC("nossvc.exe",							"nossvc"),
			//REGIST_PRG("nosstarter.npe",						NULL),
			REGIST_INST("nossvc.exe", "nos_setup.exe", "/S", "C:\\Program Files (x86)\\INCAInternet UnInstall\\nProtect Online Security", "nProtectUninstaller.exe", NULL_STR, "nProtectUninstaller.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("SGASPKILS.exe",				"SGASNXPKISVC"),
			REGIST_PRG( "KIPOCM.exe",				NULL_STR),
			REGIST_END(),
		}
	},
	{ _T("한국장학재단"),
		{
			//REGIST_SVC("nossvc.exe",							"nossvc"),
			//REGIST_PRG("nosstarter.npe",						NULL),
			REGIST_INST("nossvc.exe", "nos_setup.exe", "/S", "C:\\Program Files (x86)\\INCAInternet UnInstall\\nProtect Online Security", "nProtectUninstaller.exe", NULL_STR, "nProtectUninstaller.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("IniClientSvc_x64.exe",			"INISAFEClientManager"),
			REGIST_SVC("IniClientSvc.exe",				"INISAFEClientManager"),
			REGIST_PRG("INISAFECrossWebEXSvc.exe",		NULL_STR),
			REGIST_PRG("INISAFETrayEX.exe",				NULL_STR),

			REGIST_END(),
		}
	},
	{ _T("나이스 대국민서비스"),
		{
			//REGIST_SVC("nossvc.exe",							"nossvc"),
			//REGIST_PRG("nosstarter.npe",						NULL),
			REGIST_INST("nossvc.exe", "nos_setup.exe", "/S", "C:\\Program Files (x86)\\INCAInternet UnInstall\\nProtect Online Security", "nProtectUninstaller.exe", NULL_STR, "nProtectUninstaller.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC( "KCaseAgent.exe",		"KCaseAgent" ),
			REGIST_PRG( "KCaseLib.exe",			"C:\\Program Files (x86)\\Ksign\\KCase" ),
			REGIST_END(),
		}
	},
	{ _T("학점은행"),
		{
			REGIST_INST_END(),
		},
		{
			REGIST_PRG("UniCRSLocalServer.exe",			"C:\\Program Files (x86)\\Crosscert\\UniSignCRSV3"),
			REGIST_PRG("CCDaemon.exe",					"C:\\Program Files (x86)\\Crosscert\\UniSignCRSV3"),
			REGIST_END(),
		}
	},
	{ _T("전자소송"), 
		{
			REGIST_INST_END(),
		},
		{
			REGIST_SVC("ScourtAgent.exe",		"SCourtAgent" ),
			REGIST_SVC("innosvc.exe",			"Innosvc" ),
			REGIST_SVC("ImageSAFERSvc.exe",		"Image Protection"),

			REGIST_PRG( "ImageSAFERStart_X86.exe",		NULL_STR),
			REGIST_PRG( "ImageSAFERStart_X64.exe",		NULL_STR),
			REGIST_PRG( "CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG( "ClientSM.exe",					"C:\\Program Files (x86)\\SoftForum\\XecureWeb\\ActiveX"),

			REGIST_END(),
		}
	},
	{ _T("KB국민은행"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe					2396	Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_PRG("delfino.exe",					"C:\\Program Files (x86)\\Wizvera\\Delfino-G3"),

			REGIST_END(),
		}
	},
	{ _T("농협 인터넷뱅킹"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe					2396	Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("IniClientSvc_x64.exe",			"INISAFEClientManager"),
			REGIST_SVC("IniClientSvc.exe",				"INISAFEClientManager"),
			REGIST_SVC("ObCrossEXService.exe",			"CrossEX Live Checker"),
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("INISAFECrossWebEXSvc.exe",		NULL_STR),
			REGIST_PRG("INISAFEAdminUtil.exe",			NULL_STR),
			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),

			REGIST_END(),
		}
	},
	{ _T("IBK기업은행"),
		{
			//REGIST_SVC("nossvc.exe",							"nossvc"),
			//REGIST_PRG("nosstarter.npe",						NULL),
			REGIST_INST("nossvc.exe", "nos_setup.exe", "/S", "C:\\Program Files (x86)\\INCAInternet UnInstall\\nProtect Online Security", "nProtectUninstaller.exe", NULL_STR, "nProtectUninstaller.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("ObCrossEXService.exe",			"CrossEX Live Checker"),
			REGIST_SVC("I3GMainSvc.exe",				"Interezen_service"),
			REGIST_SVC("I3GMainSvc.exe",				"I3GMainSvc"),
			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("delfino.exe",					"C:\\Program Files (x86)\\Wizvera\\Delfino-G3"),
			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG("I3GProc.exe",					"C:\\Program Files (x86)\\IPinside_LWS"),
			REGIST_PRG("pennyroyal.exe",				"C:\\Program Files (x86)\\YettieSoft\\APS Engine"),
			REGIST_END(),
		}
	},
	{ _T("KEB하나은행"),
		{
			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("I3GMainSvc.exe",				"Interezen_service"),
			REGIST_SVC("I3GMainSvc.exe",				"I3GMainSvc"),
			REGIST_SVC("ObCrossEXService.exe",			"CrossEX Live Checker"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("delfino.exe",					"C:\\Program Files (x86)\\Wizvera\\Delfino-G3"),
			REGIST_PRG("I3GProc.exe",					"C:\\Program Files (x86)\\IPinside_LWS"),
			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG("CKAgentNXE.exe",				NULL_STR),

			REGIST_END(),
		}
	},
	{ _T("우리은행"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe					2396	Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			//REGIST_SVC("nossvc.exe",							"nossvc"),
			//REGIST_PRG("nosstarter.npe",						NULL),
			REGIST_INST("nossvc.exe",		"nos_setup.exe", "/S", "C:\\Program Files (x86)\\INCAInternet UnInstall\\nProtect Online Security", "nProtectUninstaller.exe", NULL_STR, "nProtectUninstaller.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("AnySign4PCLauncher.exe",		"AnySign4PC Launcher"),
			REGIST_SVC("I3GMainSvc.exe",				"Interezen_service"),
			REGIST_SVC("I3GMainSvc.exe",				"I3GMainSvc"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("AnySign4PC.exe",				"C:\\Program Files (x86)\\SoftForum\\XecureWeb\\AnySign\\dll"),
			REGIST_PRG("I3GProc.exe",					"C:\\Program Files (x86)\\IPinside_LWS"),

			REGIST_END(),
		}
	},
	{ _T("신한은행"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe					2396	Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("IniClientSvc_x64.exe",			"INISAFEClientManager"),
			REGIST_SVC("IniClientSvc.exe",				"INISAFEClientManager"),
			REGIST_SVC("ObCrossEXService.exe",			"CrossEX Live Checker"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("INISAFECrossWebEXSvc.exe",		NULL_STR),
			REGIST_PRG("INISAFEAdminUtil.exe",			NULL_STR),
			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG("CKAgentNXE.exe",				NULL_STR),

			REGIST_END(),
		}
	},
	{ _T("부산은행"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe					2396	Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("I3GMainSvc.exe",				"Interezen_service"),
			REGIST_SVC("I3GMainSvc.exe",				"I3GMainSvc"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("I3GProc.exe",					"C:\\Program Files (x86)\\IPinside_LWS"),
			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG("INISAFECrossWebEXSvc.exe",		NULL_STR),
			REGIST_PRG("INISAFETrayEX.exe",				NULL_STR),

			REGIST_END(),
		}
	},
	{ _T("대구은행"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe					2396	Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			//	MaWebDRMSVC.exe				2888	Services
			REGIST_INST("MaWebDRMSVC.exe",		"Inst_MaWebDRM.exe", "/S", "C:\\Program Files (x86)\\MarkAny\\WebDRMNoAX", "Uninst_WebDRM.exe", NULL_STR, "Un_A.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("ObCrossEXService.exe",			"CrossEX Live Checker"),
			REGIST_SVC("IMGSF50Svc.exe",				"IMGSF50_Svc"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG("IMGSF50Start_x64.exe",			NULL_STR),
			REGIST_PRG("IMGSF50Start_x86.exe",			NULL_STR),
			REGIST_PRG("delfino.exe",					"C:\\Program Files (x86)\\Wizvera\\Delfino-G3"),
			REGIST_PRG("MaWebDRMAgent.exe",				NULL_STR),
			REGIST_PRG("MaWebDRMAgent_x64.exe",			NULL_STR),

			REGIST_END(),
		}
	},
	{ _T("경남은행(자동감지불가)"),
		{
			//REGIST_SVC("nossvc.exe",							"nossvc"),
			//REGIST_PRG("nosstarter.npe",						NULL),
			REGIST_INST("nossvc.exe", "nos_setup.exe", "/S", "C:\\Program Files (x86)\\INCAInternet UnInstall\\nProtect Online Security", "nProtectUninstaller.exe", NULL_STR, "nProtectUninstaller.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("I3GMainSvc.exe",				"Interezen_service"),
			REGIST_SVC("I3GMainSvc.exe",				"I3GMainSvc"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("delfino.exe",					"C:\\Program Files (x86)\\Wizvera\\Delfino-G3"),
			REGIST_PRG("I3GProc.exe",					"C:\\Program Files (x86)\\IPinside_LWS"),

			REGIST_END(),
		}
	},
	{ _T("광주은행"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe					2396	Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("IniClientSvc_x64.exe",			"INISAFEClientManager"),
			REGIST_SVC("IniClientSvc.exe",				"INISAFEClientManager"),
			REGIST_SVC("I3GMainSvc.exe",				"Interezen_service"),
			REGIST_SVC("I3GMainSvc.exe",				"I3GMainSvc"),

			REGIST_SVC("ObCrossEXService.exe",			"CrossEX Live Checker"),
			REGIST_SVC("PWSLocalServer.exe",			"PSSWSCSVC"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("INISAFECrossWebEXSvc.exe",		NULL_STR),
			REGIST_PRG("INISAFEAdminUtil.exe",			NULL_STR),
			REGIST_PRG("I3GProc.exe",					"C:\\Program Files (x86)\\IPinside_LWS"),
			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),

			REGIST_END(),
		}
	},
	{ _T("전북은행"),
		{
			//REGIST_SVC("nossvc.exe",							"nossvc"),
			//REGIST_PRG("nosstarter.npe",						NULL),
			REGIST_INST("nossvc.exe", "nos_setup.exe", "/S", "C:\\Program Files (x86)\\INCAInternet UnInstall\\nProtect Online Security", "nProtectUninstaller.exe", NULL_STR, "nProtectUninstaller.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("AnySign4PCLauncher.exe",		"AnySign4PC Launcher"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("AnySign4PC.exe",				"C:\\Program Files (x86)\\SoftForum\\XecureWeb\\AnySign\\dll"),
			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG("I3GProc.exe",					"C:\\Program Files (x86)\\IPinside_LWS"),
			REGIST_PRG("CKAgentNXE.exe",				NULL_STR),
			REGIST_PRG("I3Gmgr.exe",					"C:\\Program Files (x86)\\IPinside_LWS"),

			REGIST_END(),
		}
	},
	{ _T("제주은행"),
		{
			REGIST_INST_END(),
		},
		{
			REGIST_SVC( "npkcmsvc.exe",					"npkcmsvc" ),

			REGIST_PRG( "npkagt.exe",					NULL_STR),
			REGIST_PRG( "npenksvc5.exe",				NULL_STR),
			REGIST_PRG( "npenkMain5.npn",				NULL_STR),
			REGIST_PRG( "INISAFEWeb60Tray.exe",			NULL_STR),

			REGIST_END(),
		}
	},
	{ _T("산업은행"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe					2396	Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("AnySign4PCLauncher.exe",		"AnySign4PC Launcher"),

			REGIST_PRG("AnySign4PC.exe",				"C:\\Program Files (x86)\\SoftForum\\XecureWeb\\AnySign\\dll"),

			REGIST_END(),
		}
	},
	{ _T("수협은행"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe					2396	Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("I3GMainSvc.exe",				"Interezen_service"),
			REGIST_SVC("I3GMainSvc.exe",				"I3GMainSvc"),
			REGIST_SVC("IniClientSvc_x64.exe",			"INISAFEClientManager"),
			REGIST_SVC("IniClientSvc.exe",				"INISAFEClientManager"),
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("ObCrossEXService.exe",			"CrossEX Live Checker"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("INISAFECrossWebEXSvc.exe",		NULL_STR),
			REGIST_PRG("INISAFEAdminUtil.exe",			NULL_STR),
			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG("I3GProc.exe",					"C:\\Program Files (x86)\\IPinside_LWS"),

			REGIST_END(),
		}
	},

	{ _T("우체국예금우체국보험"),
		{
			//REGIST_SVC("nossvc.exe",							"nossvc"),
			//REGIST_PRG("nosstarter.npe",						NULL),
			REGIST_INST("nossvc.exe",		"nos_setup.exe", "/S", "C:\\Program Files (x86)\\INCAInternet UnInstall\\nProtect Online Security", "nProtectUninstaller.exe", NULL_STR, "nProtectUninstaller.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("AnySign4PCLauncher.exe",		"AnySign4PC Launcher"),
			REGIST_SVC("ObCrossEXService.exe",			"CrossEX Live Checker"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("AnySign4PC.exe",				"C:\\Program Files (x86)\\SoftForum\\XecureWeb\\AnySign\\dll"),
			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG("CKAgentNXE.exe",				NULL_STR),

			REGIST_END(),
		}
	},
	{ _T("인터넷우체국"),
		{
			//REGIST_SVC("nossvc.exe",							"nossvc"),
			//REGIST_PRG("nosstarter.npe",						NULL),
			REGIST_INST("nossvc.exe",		"nos_setup.exe", "/S", "C:\\Program Files (x86)\\INCAInternet UnInstall\\nProtect Online Security", "nProtectUninstaller.exe", NULL_STR, "nProtectUninstaller.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC( "PWSLocalServer.exe", "PSSWSCSVC" ),
			REGIST_SVC( "TUCTLSystem.exe", "TUCtlSystem" ),

			REGIST_PRG( "i-FormService.exe", "C:\\Program Files (x86)\\GTONE\\iForm" ),

			REGIST_END(),
		}
	},
	{ _T("자동차민원 대국민포털"),
		{
			//REGIST_SVC("nossvc.exe",							"nossvc"),
			//REGIST_PRG("nosstarter.npe",						NULL),
			REGIST_INST("nossvc.exe",		"nos_setup.exe", "/S", "C:\\Program Files (x86)\\INCAInternet UnInstall\\nProtect Online Security", "nProtectUninstaller.exe", NULL_STR, "nProtectUninstaller.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_PRG("ClientSM.exe",					"C:\\Program Files (x86)\\SoftForum\\XecureWeb\\ActiveX"),

			REGIST_END(),
		}
	},

	{ _T("범죄경력회보서발급시스템"),
		{
			REGIST_INST_END(),
		},
		{
			REGIST_SVC("KCaseAgent.exe",				"KCaseAgent"),
			REGIST_SVC("IMGSF50Svc.exe",				"IMGSF50_Svc"),

			REGIST_PRG("KCaseLib.exe",					"C:\\Program Files (x86)\\Ksign\\KCase"),
			REGIST_PRG("IMGSF50Start_x64.exe",			NULL_STR),
			REGIST_PRG("IMGSF50Start_x86.exe",			NULL_STR),

			REGIST_END(),
		}
	},
	{ _T("경찰청교통민원24"),
		{
			REGIST_INST("TEWebP.exe",		"TouchEn_nxWeb_Installer.exe", "/S", "C:\\Program Files\\RaonSecure\\NxWeb", "uninst_32.exe", NULL_STR, "Un_A.exe"),
			REGIST_INST("TEWebP64.exe",		"TouchEn_nxWeb_Installer.exe", "/S", "C:\\Program Files\\RaonSecure\\NxWeb", "uninst.exe", NULL_STR, "Un_A.exe"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("ObCrossEXService.exe",			"CrossEX Live Checker"),
			REGIST_SVC("TENXW_SVR.exe",					"TENXW_Guard"),

			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG("CKAgentNXE.exe",				NULL_STR),

			REGIST_END(),
		}
	},

//		KISSForSMon.exe				4028    Console
	{ _T("케이뱅크"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe					2396	Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("IMGSF50Svc.exe",				"IMGSF50_Svc"),
			REGIST_SVC("I3GMainSvc.exe",				"Interezen_service"),
			REGIST_SVC("I3GMainSvc.exe",				"I3GMainSvc"),
			REGIST_SVC("IniClientSvc_x64.exe",			"INISAFEClientManager"),
			REGIST_SVC("IniClientSvc.exe",				"INISAFEClientManager"),
			REGIST_SVC("ObCrossEXService.exe",			"CrossEX Live Checker"),

			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG("I3GProc.exe",					"C:\\Program Files (x86)\\IPinside_LWS"),
			REGIST_PRG("IMGSF50Start_x64.exe",			NULL_STR),
			REGIST_PRG("IMGSF50Start_x86.exe",			NULL_STR),
			REGIST_PRG("MaEPSBroker.exe",				"C:\\Program Files (x86)\\MarkAny\\EPSBroker" ),
			REGIST_PRG("INISAFEMoaSignEX.exe",			"C:\\Program Files (x86)\\INITECH\\INISAFE MoaSign EX" ),
			REGIST_PRG( "KISSForSMon.exe",				NULL_STR),

			REGIST_END(),
		}
	},

	{ _T("컬쳐랜드"),
		{
			REGIST_INST_END(),
		},
		{
			REGIST_SVC("ObCrossEXService.exe",			"CrossEX Live Checker"),
			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG("CKAgentNXE.exe",				NULL_STR),
			REGIST_PRG("FlashUtil_ActiveX.exe",			NULL_STR),

			REGIST_END(),
		}
	},
	{ _T("해피머니"),
		{
			REGIST_INST_END(),
		},
		{
			REGIST_SVC("ObCrossEXService.exe",			"CrossEX Live Checker"),
			REGIST_PRG("CrossEXService.exe",			"C:\\Program Files (x86)\\iniLINE\\CrossEX\\crossex"),
			REGIST_PRG("CKAgentNXE.exe",				NULL_STR),
			REGIST_PRG("FlashUtil_ActiveX.exe",			NULL_STR),

			REGIST_END(),
		}
	},

	{ _T("SC제일은행"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe					2396	Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("I3GMainSvc.exe",				"Interezen_service"),
			REGIST_SVC("I3GMainSvc.exe",				"I3GMainSvc"),
			REGIST_SVC("IniClientSvc_x64.exe",			"INISAFEClientManager"),
			REGIST_SVC("IniClientSvc.exe",				"INISAFEClientManager"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("INISAFECrossWebEXSvc.exe",		NULL_STR),
			REGIST_PRG("INISAFEAdminUtil.exe",			NULL_STR),
			REGIST_PRG("I3GProc.exe",					"C:\\Program Files (x86)\\IPinside_LWS"),


			REGIST_END(),
		}
	},
	{ _T("스탠다드차타드은행"),
		{
			//ASDSvc.exe					9856	Services
			//StSess.exe					11204	Console
			//StSess32.exe					2396	Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("wpmsvc.exe",					"WizveraPMSvc"),
			REGIST_SVC("I3GMainSvc.exe",				"Interezen_service"),
			REGIST_SVC("I3GMainSvc.exe",				"I3GMainSvc"),
			REGIST_SVC("IniClientSvc_x64.exe",			"INISAFEClientManager"),
			REGIST_SVC("IniClientSvc.exe",				"INISAFEClientManager"),

			REGIST_PRG("veraport.exe",					"C:\\Program Files (x86)\\Wizvera\\Veraport20"),
			REGIST_PRG("INISAFECrossWebEXSvc.exe",		NULL_STR),
			REGIST_PRG("INISAFEAdminUtil.exe",			NULL_STR),
			REGIST_PRG("I3GProc.exe",					"C:\\Program Files (x86)\\IPinside_LWS"),


			REGIST_END(),
		}
	},
	{ _T("온비드"),
		{
			//ASDSvc.exe					4272    Services
			//StSess.exe					2640    Console
			//StSess32.exe				7376    Console
			REGIST_INST("StSess.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),
			REGIST_INST("StSess32.exe",		"astx_setup.exe", "/S", "C:\\Program Files\\AhnLab\\Safe Transaction", "V3Medic.exe", "-Uninstall", "AhnUn000.tmp"),

			REGIST_INST_END(),
		},
		{
			REGIST_SVC("AnySign4PCLauncher.exe",	"AnySign4PC Launcher"),
			REGIST_PRG("AnySign4PC.exe",			"C:\\Program Files (x86)\\SoftForum\\XecureWeb\\AnySign\\dll"),

			REGIST_END(),
		}
	},
	{ _T("문화누리"),
		{
			REGIST_INST_END(),
		},
		{
			REGIST_SVC("IniClientSvc_x64.exe",			"INISAFEClientManager"),
			REGIST_SVC("IniClientSvc.exe",				"INISAFEClientManager"),
			REGIST_PRG("INISAFECrossWebEXSvc.exe",		NULL_STR),
			REGIST_PRG("INISAFETrayEX.exe",				NULL_STR),

			REGIST_END(),
		}
	},

		/*
			{ "이름",
				{
					REGIST_INST_END(),
				},
				{
					REGIST_END(),
				}
			},
		*/

	SITE_END(),
};


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMy9RASTOPDlg 대화 상자



CMy9RASTOPDlg::CMy9RASTOPDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MY9RASTOP_DIALOG, pParent)
	, m_bWithInst(FALSE)
	, m_bWinStarExe(FALSE)
	, m_bStartWithAllStop(FALSE)
	, m_bSiteAutoDetect(FALSE)
	, m_bHideAsRun(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	initProcs();

	m_nUpdateCnt = 0;
	m_nMinSiteNameLen = 0;

	m_nDetectTimerTime = 1000;

	m_BwList.Init();

	m_bNeedBwUpdate = 0;

	for (int i = 0; i < dMAX_SITE_CNT; i++) {
		m_DetectSitePid[i] = 0;
	}
}

void CMy9RASTOPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ProcList);
	DDX_Check(pDX, IDC_CHECK1, m_bWithInst);
	DDX_Check(pDX, IDC_CHECK2, m_bWinStarExe);
	DDX_Check(pDX, IDC_CHECK3, m_bStartWithAllStop);
	DDX_Check(pDX, IDC_CHECK4, m_bSiteAutoDetect);
	DDX_Check(pDX, IDC_CHECK5, m_bHideAsRun);
	DDX_Control(pDX, IDC_COMBO1, m_DetectTimeCombo);
}

BEGIN_MESSAGE_MAP(CMy9RASTOPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMy9RASTOPDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMy9RASTOPDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CMy9RASTOPDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMy9RASTOPDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMy9RASTOPDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMy9RASTOPDlg::OnBnClickedButton4)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_MESSAGE( WM_MAIN_TRAY, &CMy9RASTOPDlg::OnMainTray)
	ON_COMMAND(IDM_OPEN_DLG, &CMy9RASTOPDlg::OnOpenDlg)
	ON_COMMAND(IDM_EXIT_MPLUG, &CMy9RASTOPDlg::OnExitMplug)
//	ON_COMMAND(IDOK, &CMy9RASTOPDlg::OnIdok)
//	ON_COMMAND(IDCANCEL, &CMy9RASTOPDlg::OnIdcancel)
ON_BN_CLICKED(IDC_BUTTON5, &CMy9RASTOPDlg::OnBnClickedButton5)
ON_BN_CLICKED(IDC_CHECK2, &CMy9RASTOPDlg::OnClickedWinStartRun)
ON_BN_CLICKED(IDC_CHECK3, &CMy9RASTOPDlg::OnClickedAllStopRun)
ON_BN_CLICKED(IDC_CHECK4, &CMy9RASTOPDlg::OnClickedSiteAutoDetect)
ON_BN_CLICKED(IDC_CHECK1, &CMy9RASTOPDlg::OnClickedWithInst)
ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &CMy9RASTOPDlg::OnNMDblclkList2)
ON_BN_CLICKED(IDC_BUTTON6, &CMy9RASTOPDlg::OnBnClickedButton6)
ON_BN_CLICKED(IDC_BUTTON7, &CMy9RASTOPDlg::OnBnClickedButton7)
ON_BN_CLICKED(IDC_CHECK5, &CMy9RASTOPDlg::OnClickedCheck5)
ON_BN_CLICKED(IDC_BUTTON8, &CMy9RASTOPDlg::OnBnClickedButton8)
ON_BN_CLICKED(IDC_BUTTON9, &CMy9RASTOPDlg::OnBnClickedButton9)
ON_CBN_SELCHANGE(IDC_COMBO1, &CMy9RASTOPDlg::OnCbnSelchangeCombo1)
ON_BN_CLICKED(IDC_BUTTON10, &CMy9RASTOPDlg::OnBnClickedButton10)
END_MESSAGE_MAP()


// CMy9RASTOPDlg 메시지 처리기

BOOL CMy9RASTOPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	//
	if (m_WmiNoti.Init(GetSafeHwnd())) {
		AfxMessageBox(_T("프로세스 감시를 시작할 수 없습니다. 사이트 자동감지는 동작하지 않습니다."));
	}

	m_Config.LoadConfig();

	m_DetectTimeCombo.SetCurSel((int)m_Config.m_dwDetectTime);
	m_nDetectTimerTime = (int)((m_Config.m_dwDetectTime + 1) * 1000);
	
	//
	if (m_Config.m_dwStartWithAllStop) {
		m_bStartWithAllStop = TRUE;
	}
	else {
		m_bStartWithAllStop = FALSE;
	}
	if (m_Config.m_dwWinStarExe) {
		m_bWinStarExe = TRUE;
	}
	else {
		m_bWinStarExe = FALSE;
	}
	if (m_Config.m_dwSiteAutoDetect) {
		m_bSiteAutoDetect = TRUE;
	}
	else {
		m_bSiteAutoDetect = FALSE;
	}
	if (m_Config.m_dwWithInst) {
		m_bWithInst = TRUE;
	}
	else {
		m_bWithInst = FALSE;
	}
	if (m_Config.m_dwHideAsRun) {
		m_bHideAsRun = TRUE;
	}
	else {
		m_bHideAsRun = FALSE;
	}

	UpdateData(FALSE);
	//

	//


	SetTimer(dSITE_DETECT_TIMER, m_nDetectTimerTime, NULL);

	CreateTray();
	m_uShellRestart = RegisterWindowMessage(_T("TrayCreate"));

	UpdateProcessInfo();
	InitProcList();
	UpdateProcList();

	
	AllStopAsRun();

	if (m_bHideAsRun == TRUE) {
		SetTimer(dDLG_HIDE_TIMER, 10, NULL);
	}


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMy9RASTOPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMy9RASTOPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMy9RASTOPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMy9RASTOPDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
}


void CMy9RASTOPDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();

	ShowWindow(SW_HIDE);
}


void CMy9RASTOPDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

// 정지
void	CMy9RASTOPDlg::StopProcess(BOOL bIsTimer) {

	if (bIsTimer == FALSE) {
		KillTimer(dSITE_DETECT_TIMER);
	}

	DisableAllBtn();

	UpdateData();

	UpdateProcessInfo();

	TCHAR szTitleMsg[512];
	Title(_T("정지작업 시작..."));

	int nStopProgCnt = 0;
	int nStopSvcCnt = 0;
	int nDelInstCnt = 0;

	for (int i = 0; i < m_ProcList.GetItemCount(); i++) {

		PumpMessage();

		if (IS_CHECKED(m_ProcList, i)) {
			CString szSiteNum = m_ProcList.GetItemText(i, 0);
			int nSiteNum = _wtoi((TCHAR*)szSiteNum.operator LPCWSTR());
			szSiteNum.Empty();

			sPSITE_INFO pSite = (sPSITE_INFO)&g_sSiteInfo[nSiteNum];

			// 보안툴 
			int nInstNum = 0;

			if (m_bWithInst) {
				while (1) {
					PumpMessage();

					sPONE_INST_INFO pInst = (sPONE_INST_INFO)&g_sSiteInfo[nSiteNum].m_InstInfo[nInstNum];

					if (pInst->m_szCmdName == NULL)
						break;

					if (findProcs(pInst->m_szCmdName) == TRUE &&
						findProcs(pInst->m_szRealUninstaller) == FALSE) {

						wsprintf(szTitleMsg, _T("'%s'의 '%s'를 제거하는중"), pSite->m_szSiteName, pInst->m_szCmdName);
						Title(szTitleMsg);

						TCHAR checkInstPath[MAX_PATH];
						TCHAR instDir32[MAX_PATH];
						TCHAR instDir64[MAX_PATH];

						*checkInstPath = NULL;
						*instDir32 = NULL;
						*instDir64 = NULL;

						if (makeBitDir(pInst->m_szUninstallDir, instDir32, instDir64) == FALSE) {
							wsprintf(checkInstPath, _T("%s\\%s"), instDir32, pInst->m_szUninstaller);

							if (GetFileAttributes(checkInstPath) != -1) {

								Exec(pInst->m_szUninstaller, pInst->m_szUninstParam, instDir32, FALSE, TRUE);

								waitProcess(pInst->m_szUninstaller);
								waitProcess(pInst->m_szRealUninstaller);

								UpdateProcessInfo();

								nDelInstCnt++;
							}
						}
						else {
							wsprintf(checkInstPath, _T("%s\\%s"), instDir32, pInst->m_szUninstaller);

							if (GetFileAttributes(checkInstPath) != -1) {

								Exec(pInst->m_szUninstaller, pInst->m_szUninstParam, instDir32, FALSE, TRUE);

								waitProcess(pInst->m_szUninstaller);
								waitProcess(pInst->m_szRealUninstaller);

								UpdateProcessInfo();

								nDelInstCnt++;
							}
							else {
								wsprintf(checkInstPath, _T("%s\\%s"), instDir64, pInst->m_szUninstaller);

								if (GetFileAttributes(checkInstPath) != -1) {

									Exec(pInst->m_szUninstaller, pInst->m_szUninstParam, instDir64, FALSE, TRUE);

									waitProcess(pInst->m_szUninstaller);
									waitProcess(pInst->m_szRealUninstaller);

									UpdateProcessInfo();

									nDelInstCnt++;
								}
							}
						}

					}
					nInstNum++;
				}
			}

			// 서비스, 프로그램 
			int nProgNum = 0;

			while (1) {

				PumpMessage();

				sPONE_PRC_INFO pPrc = (sPONE_PRC_INFO)&g_sSiteInfo[nSiteNum].m_PrcInfo[nProgNum];

				if (pPrc->m_szCmdName == NULL)
					break;

				// 프로그램
				if (pPrc->m_szSvcName == NULL) {
					if (findProcs(pPrc->m_szCmdName) == TRUE) {

						wsprintf(szTitleMsg, _T("'%s'의 '%s'를 죽이는중"), pSite->m_szSiteName, pPrc->m_szCmdName);
						Title(szTitleMsg);

						TCHAR szPara[128];

						wsprintf(szPara, _T("/IM \"%s\" /F"), pPrc->m_szCmdName);

						Exec(_T("taskkill.exe"), szPara, NULL, TRUE);

						nStopProgCnt++;

						UpdateProcessInfo();

					}

				} // 서비스 
				else {
					if (findProcs(pPrc->m_szCmdName) == TRUE) {

						if (isExistService(pPrc->m_szSvcName) == TRUE) {

							if (getServiceState(pPrc->m_szSvcName) == SERVICE_RUNNING) {
								wsprintf(szTitleMsg, _T("'%s'의 '%s' 서비스를 중지중"), pSite->m_szSiteName, pPrc->m_szSvcName);
								Title(szTitleMsg);

								SafeServiceStop(pPrc->m_szSvcName);

								nStopSvcCnt++;

								UpdateProcessInfo();
							}
						}
					}
				}

				nProgNum++;
			}
		}
	}
	UpdateProcList2();
	StartUpdateTimer();

	wsprintf(szTitleMsg, _T("%d개의 프로그램과 %d개의 서비스가 정지됨. (%d 보안툴 삭제)"), nStopProgCnt, nStopSvcCnt, nDelInstCnt);
	Title(szTitleMsg);

	EnableAllBtn();

	if (bIsTimer == FALSE) {
		SetTimer(dSITE_DETECT_TIMER, m_nDetectTimerTime, NULL);
	}
}

// 정지
void CMy9RASTOPDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	StopProcess(FALSE);
}

// 시작
void	CMy9RASTOPDlg::StartProcess(BOOL bIsTimer) {
	if (bIsTimer == FALSE) {
		KillTimer(dSITE_DETECT_TIMER);
	}

	DisableAllBtn();

	UpdateData();

	UpdateProcessInfo();

	TCHAR szTitleMsg[512];
	Title(_T("시작작업 시작..."));

	int nRunProgCnt = 0;
	int nRunSvcCnt = 0;
	int nRunInstCnt = 0;

	for (int i = 0; i < m_ProcList.GetItemCount(); i++) {
		PumpMessage();

		if (IS_CHECKED(m_ProcList, i)) {
			CString szSiteNum = m_ProcList.GetItemText(i, 0);
			int nSiteNum = _wtoi((TCHAR*)szSiteNum.operator LPCWSTR());
			szSiteNum.Empty();

			sPSITE_INFO pSite = (sPSITE_INFO)&g_sSiteInfo[nSiteNum];

			int nProgNum = 0;
			int nInstNum = 0;

			if (m_bWithInst) {
				// 보안툴
				while (1) {
					PumpMessage();

					sPONE_INST_INFO pInst = (sPONE_INST_INFO)&g_sSiteInfo[nSiteNum].m_InstInfo[nInstNum];

					if (pInst->m_szCmdName == NULL)
						break;

					if (findProcs(pInst->m_szCmdName) == FALSE &&
						findProcs(pInst->m_szInstName) == FALSE) {

						wsprintf(szTitleMsg, _T("'%s'의 '%s'를 설치하는중"), g_sSiteInfo[nSiteNum].m_szSiteName, pInst->m_szCmdName);
						Title(szTitleMsg);

						TCHAR szInstFile[MAX_PATH];
						TCHAR szInstPath[MAX_PATH];
						TCHAR szCurrDir[MAX_PATH];

						GetCurrentDirectory(MAX_PATH, szCurrDir);

						wsprintf(szInstPath, _T("%s\\install"), szCurrDir);
						wsprintf(szInstFile, _T("%s\\install\\%s"), szCurrDir, pInst->m_szInstName);

						if (GetFileAttributes(szInstFile) != -1) {
							Exec(pInst->m_szInstName, pInst->m_szInstParam, szInstPath, FALSE, TRUE);
							waitProcess(pInst->m_szInstName);
							nRunInstCnt++;

							UpdateProcessInfo();
						}
					}

					nInstNum++;
				}
			}

			// 서비스, 프로그램
			while (1) {
				PumpMessage();

				sPONE_PRC_INFO pPrc = (sPONE_PRC_INFO)&g_sSiteInfo[nSiteNum].m_PrcInfo[nProgNum];

				if (pPrc->m_szCmdName == NULL)
					break;

				// 프로그램
				if (pPrc->m_szSvcName == NULL) {

					if (findProcs(pPrc->m_szCmdName) == FALSE) {

						if (pPrc->m_szStartDir != NULL) {

							wsprintf(szTitleMsg, _T("'%s'의 '%s'를 실행시키는중"), pSite->m_szSiteName, pPrc->m_szCmdName);
							Title(szTitleMsg);

							TCHAR dir32[MAX_PATH];
							TCHAR dir64[MAX_PATH];

							*dir32 = NULL;
							*dir64 = NULL;

							TCHAR szCheckFile[MAX_PATH];

#define RUN_PROG( dir ) {Exec(pPrc->m_szCmdName, NULL, dir, FALSE, FALSE);\
nRunProgCnt++;\
UpdateProcessInfo();}

							if (makeBitDir(pPrc->m_szStartDir, dir32, dir64) == FALSE) {
								wsprintf(szCheckFile, _T("%s\\%s"), dir32, pPrc->m_szCmdName);
								if (GetFileAttributes(szCheckFile) != -1) {
									RUN_PROG(dir32);
								}
							}
							else {
								wsprintf(szCheckFile, _T("%s\\%s"), dir32, pPrc->m_szCmdName);
								if (GetFileAttributes(szCheckFile) != -1) {
									RUN_PROG(dir32);
								}
								else {
									wsprintf(szCheckFile, _T("%s\\%s"), dir64, pPrc->m_szCmdName);
									if (GetFileAttributes(szCheckFile) != -1) {
										RUN_PROG(dir64);
									}
								}
							}
						}
					}
				} // 서비스 
				else {

					if (findProcs(pPrc->m_szCmdName) == FALSE) {

						if (isExistService(pPrc->m_szSvcName) == TRUE) {
							if (getServiceState(pPrc->m_szSvcName) == SERVICE_STOPPED) {
								wsprintf(szTitleMsg, _T("'%s'의 '%s' 서비스를 시작중..."), pSite->m_szSiteName, pPrc->m_szSvcName);
								Title(szTitleMsg);

								ServiceStart(pPrc->m_szSvcName);

								nRunSvcCnt++;

								UpdateProcessInfo();
							}
						}
					}
				}

				nProgNum++;
			}
		}
	}

	UpdateProcList2();
	StartUpdateTimer();

	wsprintf(szTitleMsg, _T("%d개의 프로그램과 %d개의 서비스가 시작됨.(%d 보안툴 설치)"), nRunProgCnt, nRunSvcCnt, nRunInstCnt);
	Title(szTitleMsg);

	EnableAllBtn();

	if (bIsTimer == FALSE) {
		SetTimer(dSITE_DETECT_TIMER, m_nDetectTimerTime, NULL);
	}
}

// 시작
void CMy9RASTOPDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	StartProcess(FALSE);
}

void	CMy9RASTOPDlg::InitProcList() {
	m_ProcList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER |LVS_EX_CHECKBOXES);

	LV_COLUMN                lvcolumn;

	sITEM_CONFIG		list_config[] =
	{
		{ _T("#"),				50,			LVCFMT_LEFT, },
		{ _T("사이트명"),		480,		LVCFMT_LEFT, },
		{ _T("프로그램"),		100,			LVCFMT_LEFT, },
		{ _T("서비스"),			100,			LVCFMT_LEFT, },
		{ _T("보안툴"),			100,			LVCFMT_LEFT, },

		{ NULL, 0, 0 },
	};

	int nLine = 0;

	while (1)
	{
		if (list_config[nLine].m_szTitle == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = list_config[nLine].m_nFmt;
		lvcolumn.pszText = list_config[nLine].m_szTitle;
		lvcolumn.iSubItem = nLine;
		lvcolumn.cx = list_config[nLine].m_nWidth;

		m_ProcList.InsertColumn(nLine, &lvcolumn);

		nLine++;
	}

	//

	
}

void	CMy9RASTOPDlg::UpdateProcList2() {
	for (int i = 0; i < m_ProcList.GetItemCount(); i++) {
		CString szSiteNum = m_ProcList.GetItemText(i, 0);
		int nSiteNum = _wtoi(szSiteNum);
		szSiteNum.Empty();

		INIT_LVITEM(m_ProcList);
		UPDATE_LVITEM(m_ProcList, i, 2, _T("%d"), getProgCnt(nSiteNum));
		UPDATE_LVITEM(m_ProcList, i, 3, _T("%d"), getServiceCnt(nSiteNum));
		UPDATE_LVITEM(m_ProcList, i, 4, _T("%d"), getInstCnt(nSiteNum));
	}
}

void	CMy9RASTOPDlg::UpdateProcList() {

	m_ProcList.DeleteAllItems();

	int nSiteCnt = 0;

	TCHAR szIgnoreNum[128];

	while (1) {

		if (g_sSiteInfo[nSiteCnt].m_szSiteName == NULL)
			break;

		if (wcslen(m_Config.m_szIgnore) > 0) {
			wsprintf(szIgnoreNum, _T("[%d]"), nSiteCnt);
			if (wcsstr(m_Config.m_szIgnore, szIgnoreNum))
			{
				nSiteCnt++;
				continue;
			}
		}

		int nLen = wcslen(g_sSiteInfo[nSiteCnt].m_szSiteName);

		if (m_nMinSiteNameLen == 0) {
			m_nMinSiteNameLen = nLen;
		}
		else {
			
			if (m_nMinSiteNameLen > nLen)
				m_nMinSiteNameLen = nLen;
		}

		INIT_LVITEM(m_ProcList);

		SET_LVITEM(m_ProcList, _T("%d"), nSiteCnt);
		SET_LVITEM(m_ProcList, _T("%s"), g_sSiteInfo[nSiteCnt].m_szSiteName);

		SET_LVITEM(m_ProcList, _T("%d"), getProgCnt(nSiteCnt));
		SET_LVITEM(m_ProcList, _T("%d"), getServiceCnt(nSiteCnt));

		SET_LVITEM(m_ProcList, _T("%d"), getInstCnt(nSiteCnt));
		
		nSiteCnt++;

		if (nSiteCnt >= dMAX_SITE_CNT) {
			AfxMessageBox(_T("최대 사이트 수 오버 오류!!"));
			exit(1);
		}
	}

}


void	CMy9RASTOPDlg::UpdateProcessInfo() {
	initProcs();

	HANDLE hSnapShot;
	PROCESSENTRY32 pEntry;

	// TH32CS_SNAPALL 옵션으로 모든 프로세스의 정보를 얻어냅니다.
	hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

	pEntry.dwSize = sizeof(pEntry);

	// Process32First 함수를 통해서 SnapShot에 들어 있는 첫번째 프로세스를 받아옵니다.
	Process32First(hSnapShot, &pEntry);

	int nProcsCnt = 0;

	while (true) {
		// Process32Next 함수를 통해서 pEntry 의 다음 프로세스를 스냅샷에서 받아 옵니다.
		BOOL hRes = Process32Next(hSnapShot, &pEntry);

		if (hRes == FALSE)
			break;

		if (wcscmp(pEntry.szExeFile, _T("svchost.exe")) &&
			wcscmp(pEntry.szExeFile, _T("cmd.exe")) &&
			wcscmp(pEntry.szExeFile, _T("tasklist.exe")) &&
			wcscmp(pEntry.szExeFile, _T("chrome.exe")) &&
			wcscmp(pEntry.szExeFile, _T("RuntimeBroker.exe")) &&
			wcscmp(pEntry.szExeFile, _T("MicrosoftEdge.exe")) &&
			wcscmp(pEntry.szExeFile, _T("dllhost.exe")) &&
			wcscmp(pEntry.szExeFile, _T("explorer.exe")) &&
			wcscmp(pEntry.szExeFile, _T("ApplicationFrameHost.exe")) &&
			wcscmp(pEntry.szExeFile, _T("MicrosoftEdgeSH.exe")) &&
			wcscmp(pEntry.szExeFile, _T("MicrosoftEdgeCP.exe")) &&
			wcscmp(pEntry.szExeFile, _T("V3Lite.exe")) &&
			wcscmp(pEntry.szExeFile, _T("SystemSettings.exe")) &&
			wcscmp(pEntry.szExeFile, _T("GoogleUpdate.exe")) &&
			wcscmp(pEntry.szExeFile, _T("conhost.exe"))) {

			wcscpy(m_szProcs[nProcsCnt], pEntry.szExeFile);

			nProcsCnt++;
		}

	}
}


void	CMy9RASTOPDlg::initProcs() {
	for (int i = 0; i < dMAX_PROCS; i++) {
		*m_szProcs[i] = NULL;
	}
}

BOOL	CMy9RASTOPDlg::findProcs(TCHAR *szProcsName) {
	for (int i = 0; i < dMAX_PROCS; i++) {
		if (*m_szProcs[i] == NULL) {
			break;
		}

		if (!wcscmp(m_szProcs[i], szProcsName)) {
			return TRUE;
		}
	}

	return FALSE;
}



int     CMy9RASTOPDlg::getServiceCnt(int nSiteIndex) {

	int nProgCnt = 0;
	int nRetCnt = 0;

	while (1) {
		if (g_sSiteInfo[nSiteIndex].m_PrcInfo[nProgCnt].m_szCmdName == NULL)
			break;

		if (g_sSiteInfo[nSiteIndex].m_PrcInfo[nProgCnt].m_szSvcName != NULL &&
			findProcs(g_sSiteInfo[nSiteIndex].m_PrcInfo[nProgCnt].m_szCmdName) )
			nRetCnt++;

		nProgCnt++;
	}
	return nRetCnt;
}

int     CMy9RASTOPDlg::getProgCnt(int nSiteIndex) {
	int nProgCnt = 0;
	int nRetCnt = 0;

	while (1) {
		if (g_sSiteInfo[nSiteIndex].m_PrcInfo[nProgCnt].m_szCmdName == NULL)
			break;

		if (g_sSiteInfo[nSiteIndex].m_PrcInfo[nProgCnt].m_szSvcName == NULL &&
			findProcs(g_sSiteInfo[nSiteIndex].m_PrcInfo[nProgCnt].m_szCmdName))
			nRetCnt++;

		nProgCnt++;
	}
	return nRetCnt;
}

int CMy9RASTOPDlg::getInstCnt(int nSiteIndex) {
	int nProgCnt = 0;
	int nRetCnt = 0;

	while (1) {
		if (g_sSiteInfo[nSiteIndex].m_InstInfo[nProgCnt].m_szCmdName == NULL)
			break;

		if (findProcs(g_sSiteInfo[nSiteIndex].m_InstInfo[nProgCnt].m_szCmdName))
			nRetCnt++;

		nProgCnt++;
	}
	return nRetCnt;
}

// 전체선택 
void CMy9RASTOPDlg::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_ProcList.GetItemCount() == 0)
		return;


	for (int i = 0; i < m_ProcList.GetItemCount(); i++) {
		ON_CHECKED(m_ProcList, i);
	}

}

// 전체해제
void CMy9RASTOPDlg::OnBnClickedButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_ProcList.GetItemCount() == 0)
		return;

	for (int i = 0; i < m_ProcList.GetItemCount(); i++) {
		OFF_CHECKED(m_ProcList, i);
	}
}


void	CMy9RASTOPDlg::Exec(TCHAR *cmd, TCHAR *param, TCHAR *dir, BOOL bWait, BOOL bShow ) {
	//
	SHELLEXECUTEINFO sei = { 0 };
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = _T("runas");
	sei.lpFile = cmd;
	sei.lpDirectory = dir;
	sei.lpParameters = param;
	if (bShow == TRUE) {
		sei.nShow = SW_SHOWNORMAL;
	}
	else {
		sei.nShow = SW_HIDE;
	}

	if (ShellExecuteEx(&sei))
	{
		if (bWait == TRUE) {
			WaitForSingleObject(sei.hProcess, INFINITE);
		}
	}
	//
}


int CMy9RASTOPDlg::ServiceStart(TCHAR * serviceName)
{
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	//1. 서비스를 오픈한다.
	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // servicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		DWORD  dwError = ::GetLastError();
		return dwError;
	}

	//2. 서비스 이름으로 특정 서비스를 오픈한다.
	schService = OpenService(
		schSCManager,         // SCM database 
		serviceName,          // name of service 
		SERVICE_ALL_ACCESS);  // full access 

	if (schService == NULL)
	{
		CloseServiceHandle(schSCManager);

		DWORD  dwError = ::GetLastError();
		return dwError;
	}

	//3. 서비스를 시작한다.
	BOOL bStart = ::StartService(schService, 0, NULL);
	if (bStart == FALSE)
	{
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);

		DWORD  dwError = ::GetLastError();
	
		return dwError;
	}

	//4. 서비스 핸들은 모두 닫아준다.
	CloseServiceHandle(schSCManager);
	CloseServiceHandle(schService);

	return 0;
}


int CMy9RASTOPDlg::ServiceStop(TCHAR * serviceName)
{
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	//1. 서비스를 오픈한다.
	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // servicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		DWORD  dwError = ::GetLastError();
		return dwError;
	}

	//2. 서비스 이름을 이용해서 특정 서비스를 오픈한다.
	schService = OpenService(
		schSCManager,         // SCM database 
		serviceName,          // name of service 
		SERVICE_ALL_ACCESS);  // full access 

	if (schService == NULL)
	{
		CloseServiceHandle(schSCManager);

		DWORD  dwError = ::GetLastError();

		return dwError;
	}


	//3. 서비스의 상태를 요청한다.
	SERVICE_STATUS ss;

	BOOL bQuery = ::QueryServiceStatus(schService, &ss);
	if (bQuery == FALSE)
	{
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);

		DWORD  dwError = ::GetLastError();

		return dwError;
	}

	//4. 서비스 상태가 STOP 상태가 아니라면 STOP을 요청한다.
	if (ss.dwCurrentState != SERVICE_STOPPED)
	{
		BOOL bControl = ::ControlService(schService, SERVICE_CONTROL_STOP, &ss);
		if (bControl == FALSE)
		{
			CloseServiceHandle(schSCManager);
			CloseServiceHandle(schService);

			DWORD  dwError = ::GetLastError();

			return dwError;

		}
	}


	//5. 서비스 핸들은 모두 닫아준다.
	CloseServiceHandle(schSCManager);
	CloseServiceHandle(schService);

	return 0;
}

BOOL	CMy9RASTOPDlg::isExistService(TCHAR * serviceName) {
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	//1. 서비스를 오픈한다.
	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // servicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		return FALSE;
	}

	//2. 서비스 이름을 이용해서 특정 서비스를 오픈한다.
	schService = OpenService(
		schSCManager,         // SCM database 
		serviceName,          // name of service 
		SERVICE_ALL_ACCESS);  // full access 

	if (schService == NULL)
	{
		return FALSE;
	}

	CloseServiceHandle(schSCManager);
	CloseServiceHandle(schService);
	return TRUE;
}

DWORD	CMy9RASTOPDlg::getServiceState(TCHAR * serviceName) {
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	//1. 서비스를 오픈한다.
	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // servicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		DWORD  dwError = ::GetLastError();
		return dwError;
	}

	//2. 서비스 이름을 이용해서 특정 서비스를 오픈한다.
	schService = OpenService(
		schSCManager,         // SCM database 
		serviceName,          // name of service 
		SERVICE_ALL_ACCESS);  // full access 

	if (schService == NULL)
	{
		CloseServiceHandle(schSCManager);

		DWORD  dwError = ::GetLastError();

		return dwError;
	}


	//3. 서비스의 상태를 요청한다.
	SERVICE_STATUS ss;

	BOOL bQuery = ::QueryServiceStatus(schService, &ss);

	if (bQuery == FALSE)
	{
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);

		DWORD  dwError = ::GetLastError();

		return dwError;
	}


	//5. 서비스 핸들은 모두 닫아준다.
	CloseServiceHandle(schSCManager);
	CloseServiceHandle(schService);

	return ss.dwCurrentState;
}




int		CMy9RASTOPDlg::SafeServiceStop(TCHAR * serviceName) {

	while (getServiceState(serviceName) != SERVICE_STOPPED) {
		PumpMessage();
		ServiceStop(serviceName);
	}
	return 0;
}


void CMy9RASTOPDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (nIDEvent == dUPDATE_TIMER_ID) {
		KillTimer(dUPDATE_TIMER_ID);

		m_nUpdateCnt++;

		if (m_nUpdateCnt < 5)
		{
			UpdateProcessInfo();
			UpdateProcList2();
			SetTimer(dUPDATE_TIMER_ID, 1000, NULL);
		}
	}
	else if (nIDEvent == dSITE_DETECT_TIMER) {
		KillTimer(dSITE_DETECT_TIMER);

		if (m_bNeedBwUpdate) {
			UpdateBW();
			m_bNeedBwUpdate = 0;
		}

		// 옵션이 켜져 있으면
		if (m_bSiteAutoDetect) {
			DetectSiteProc();
		}
		//

		SetTimer(dSITE_DETECT_TIMER, m_nDetectTimerTime, NULL);
	}
	else if (nIDEvent == dDLG_HIDE_TIMER) {
		KillTimer(dDLG_HIDE_TIMER);
		ShowWindow(SW_HIDE);
	}

	CDialogEx::OnTimer(nIDEvent);
}

void	CMy9RASTOPDlg::StartUpdateTimer() {
	m_nUpdateCnt = 0;

	SetTimer(dUPDATE_TIMER_ID, 1000, NULL);
}



int CMy9RASTOPDlg::PumpMessage()
{
	// TODO: 여기에 구현 코드 추가.
	MSG msg;
	if (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (!AfxGetApp()->PumpMessage()) {
			::PostQuitMessage(0);
			return FALSE;
		}
	}
	return 1;
}


void	CMy9RASTOPDlg::Title(TCHAR *szMsg) {

	TCHAR szTitle[512];

	if (szMsg != NULL) {
		wsprintf(szTitle, _T("%s[%d] - %s"), _T(dPROG_NAME), m_BwList.GetSize(), szMsg);
		SetWindowText(szTitle);
	}
	else {
		SetWindowText(_T(dPROG_NAME));
	}

}

BOOL	CMy9RASTOPDlg::makeBitDir(TCHAR *src, TCHAR *dir32, TCHAR *dir64) {
	if (!wcsstr(src, _T("Program Files (x86)"))) {
		wcscpy(dir32, src);
		wcscpy(dir64, src);
		return FALSE;
	}

	CString tmp = src;

	wcscpy(dir32, src);

	tmp.Replace(_T("Program Files (x86)"), _T("Program Files"));

	wcscpy(dir64, (TCHAR*)tmp.operator LPCWSTR());

	tmp.Empty();

	return TRUE;
}

void	CMy9RASTOPDlg::waitProcess(TCHAR *szProcName) {

	while (1) {
		PumpMessage();
		UpdateProcessInfo();
		if (findProcs(szProcName) == FALSE)
			break;
		Sleep(100);
	}

}

void	CMy9RASTOPDlg::CreateTray() {
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));

	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage = WM_MAIN_TRAY;
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	_tcscpy_s(nid.szTip, _T("모두의플러그"));

	Shell_NotifyIcon(NIM_ADD, &nid);
}

void	CMy9RASTOPDlg::DestroyTray() {
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));

	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDR_MAINFRAME;

	Shell_NotifyIcon(NIM_DELETE, &nid);
}


void CMy9RASTOPDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_WmiNoti.Destroy();
	m_BwList.DestroyWithData();

	m_Config.SaveConfig();
	DestroyTray();
}


afx_msg LRESULT CMy9RASTOPDlg::OnMainTray(WPARAM wParam, LPARAM lParam) {
	switch (lParam) {
	case WM_LBUTTONDBLCLK:
		ShowWindow(SW_SHOW);
		SetForegroundWindow();
		break;
	case WM_RBUTTONDOWN:
		SetCapture();
		break;
	case WM_RBUTTONUP:
		CMenu menu, *pMenu = NULL;
		CPoint pt;

		menu.LoadMenu(IDR_MENU1);
		pMenu = menu.GetSubMenu(0);

		GetCursorPos(&pt);

		SetForegroundWindow();

		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
		::PostMessage(m_hWnd, WM_NULL, NULL, NULL);

		ReleaseCapture();

		break;
	}

	return 0;
}

LRESULT CMy9RASTOPDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	 // 윈도우 탐색기가 종료됐을때, 모든 트레이가 사라지므로 재등록
	if (message == m_uShellRestart) 
		CreateTray();
	else if (message == WM_NEW_BROWSER) {
		m_bNeedBwUpdate = 1;
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}


void CMy9RASTOPDlg::OnOpenDlg()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ShowWindow(SW_SHOW);
}


void CMy9RASTOPDlg::OnExitMplug()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDialog::OnOK();
}



void CMy9RASTOPDlg::OnBnClickedButton5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnOK();
}


BOOL	CMy9RASTOPDlg::RegistStartUp(BOOL is64Bit) {
	LONG lResult;
	HKEY hkey;

	if (!is64Bit) {
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hkey);
	}
	else {
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hkey);
	}

	if (ERROR_SUCCESS == lResult) {

		TCHAR currDir[MAX_PATH];
		TCHAR path[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currDir);
		wsprintf(path, _T("%s\\%s"), currDir, dMPLUG_EXE_NAME);

		// 레지스트리의 run 항목에 자동 실행될 프로그램의 경로를 저장한다.
		lResult = RegSetValueEx(hkey, _T("MODU_PLUG"), 0, REG_SZ, (BYTE *)path, (DWORD)wcslen(path) + 1);

		if (lResult == ERROR_SUCCESS) {
			// 오픈한 키를 닫는다.
			RegCloseKey(hkey);
			return TRUE;
		}
		else {
			RegCloseKey(hkey);
			return FALSE;
		}
	}
	return FALSE;
}
BOOL	CMy9RASTOPDlg::UnRegistStartUp(BOOL is64Bit) {
	LONG lResult;
	HKEY hKey;

	if (!is64Bit) {
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey);
	}
	else {
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey);
	}

	if (lResult == ERROR_SUCCESS)
	{
		lResult = RegDeleteValue(hKey, _T("MODU_PLUG"));      // 삭제

		if (lResult == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return TRUE;
		}
		else
		{
			RegCloseKey(hKey);
			return FALSE;
		}
	}
	return FALSE;
}

void CMy9RASTOPDlg::OnClickedWinStartRun()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	m_Config.m_dwWinStarExe = m_bWinStarExe;

	m_Config.SaveConfig();

	// 윈도 시작시 시작 
	if (m_bWinStarExe) {

		if (!RegistStartUp(TRUE)) {
			if (!RegistStartUp(FALSE)) {
				Title(_T("시작프로그램 등록 실패"));
			}
			else {
				Title(_T("시작프로그램 등록 성공"));
			}
		}
		else {
			Title(_T("시작프로그램 등록 성공"));
		}
	}
	else {
		if (!UnRegistStartUp(TRUE)) {
			if (!UnRegistStartUp(FALSE)) {
				Title(_T("시작프로그램 제거 실패"));
			}
			else {
				Title(_T("시작프로그램 제거 성공"));
			}
		}
		else {
			Title(_T("시작프로그램 제거 성공"));
		}
	}

	//

}


void CMy9RASTOPDlg::OnClickedAllStopRun()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	m_Config.m_dwStartWithAllStop = m_bStartWithAllStop;

	m_Config.SaveConfig();
}


void CMy9RASTOPDlg::OnClickedSiteAutoDetect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	m_Config.m_dwSiteAutoDetect = m_bSiteAutoDetect;

	m_Config.SaveConfig();
}


void CMy9RASTOPDlg::OnClickedWithInst()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	m_Config.m_dwWithInst = m_bWithInst;

	m_Config.SaveConfig();
}

// 실행시 창 숨김
void CMy9RASTOPDlg::OnClickedCheck5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	m_Config.m_dwHideAsRun = m_bHideAsRun;

	m_Config.SaveConfig();
}




void	CMy9RASTOPDlg::AllStopAsRun() {
	UpdateData();

	if (m_bStartWithAllStop) {
		for (int i = 0; i < m_ProcList.GetItemCount(); i++) {
			ON_CHECKED(m_ProcList, i);
		}
		OnBnClickedButton1();
	}
}

void CMy9RASTOPDlg::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;	

	if (m_ProcList.GetSelectedCount() == 1) {

		if (IS_CHECKED(m_ProcList, m_ProcList.GetSelectionMark())) {
			OFF_CHECKED(m_ProcList, m_ProcList.GetSelectionMark());
		}
		else {
			ON_CHECKED(m_ProcList, m_ProcList.GetSelectionMark());
		}

	}
}

int					CMy9RASTOPDlg::FindSite(TCHAR *szBwTitle) {
	for (int i = 0; i < m_ProcList.GetItemCount(); i++) {
		CString szTmp = m_ProcList.GetItemText(i, 1);
		TCHAR szName[512];
		wcscpy(szName, (TCHAR*)szTmp.operator LPCWSTR());
		szTmp = m_ProcList.GetItemText(i, 0);
		int nSiteNum = _wtol((TCHAR*)szTmp.operator LPCWSTR());
		szTmp.Empty();

		if (wcsstr(szBwTitle, szName)) {
			return nSiteNum;
		}
	}
	return -1;
}




void	CMy9RASTOPDlg::DetectSiteProc() {

	CString szTmp;

	sPLIST_BLOCK list, block, next_block;

	list = m_BwList.GetFirstBlock();

	LIST_WHILE(list, block, next_block, next);

	sPBW_DATA pData = (sPBW_DATA)block->data;

	// 윈도가 살아 있으면 
	if (::IsWindow(pData->m_hWnd)) {
		TCHAR szTitle[512];
		::GetWindowText(pData->m_hWnd, szTitle, 512);

		int nSiteNum = FindSite(szTitle);

		// 감지된 사이트가 있으면
		if (nSiteNum != -1) {
			// 감지된적 없으면 추가

			if (m_DetectSitePid[nSiteNum] == 0) {
	
				m_DetectSitePid[nSiteNum] = pData->m_dwPid;

				for (int si = 0; si < m_ProcList.GetItemCount(); si++) {
					szTmp = m_ProcList.GetItemText(si, 0);
					int nSiSiteNum = _wtoi((TCHAR*)szTmp.operator LPCWSTR());
					
					if (nSiSiteNum == nSiteNum) {
						ON_CHECKED(m_ProcList, si);
					}
					else {
						OFF_CHECKED(m_ProcList, si);
					}
				}
				StartProcess(TRUE);
			}
		}
	}
	else {	// 죽었으면 

		if (hasDetectSite(pData->m_dwPid)) {
			for (int si = 0; si < m_ProcList.GetItemCount(); si++) {
				szTmp = m_ProcList.GetItemText(si, 0);
				int nSiSiteNum = _wtoi((TCHAR*)szTmp.operator LPCWSTR());

				if (m_DetectSitePid[nSiSiteNum] == pData->m_dwPid) {
					m_DetectSitePid[nSiSiteNum] = 0;
					ON_CHECKED(m_ProcList, si);
				}
				else {
					OFF_CHECKED(m_ProcList, si);
				}
			}
			StopProcess(TRUE);
		}

		m_BwList.remove(block);
		SAFE_FREE(pData);
		LIST_SKIP();

		UpdateBwCntStatic();

	}
	LIST_WHILEEND();

	szTmp.Empty();
}

void CMy9RASTOPDlg::OnBnClickedButton6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DisableAllBtn();
	UpdateProcessInfo();
	UpdateProcList2();
	EnableAllBtn();
}


// 후원안내 
void CMy9RASTOPDlg::OnBnClickedButton7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	ShellExecute(NULL, _T("open"), _T("https://modu-print.tistory.com/93"),NULL, NULL, 0);

}


// 리스트 초기화
void CMy9RASTOPDlg::OnBnClickedButton8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (MessageBox(_T("사이트 리스트를 초기상태로 초기화 합니다."), _T("알림"), MB_YESNO) == IDNO) {
		return;
	}

	m_Config.m_szIgnore[0] = NULL;

	m_Config.SaveConfig();

	UpdateProcList();
}

// 선택 제외
void CMy9RASTOPDlg::OnBnClickedButton9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (MessageBox(_T("체크한 사이트를 리스트에서 제외합니다."), _T("알림"), MB_YESNO) == IDNO) {
		return;
	}

	TCHAR szNum[128];

	for (int i = 0; i < m_ProcList.GetItemCount(); i++) {

		if (IS_CHECKED(m_ProcList, i)) {
			CString szSiteNum = m_ProcList.GetItemText(i, 0);
			int nSIteNum = _wtoi(szSiteNum);
			szSiteNum.Empty();

			wsprintf(szNum, _T("[%d]"), nSIteNum);

			if(!wcsstr(m_Config.m_szIgnore, szNum)) {
				wcscat(m_Config.m_szIgnore, szNum);
			}
		}
	}

	m_Config.SaveConfig();
	UpdateProcList();
}


void CMy9RASTOPDlg::OnCbnSelchangeCombo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData();

	m_Config.m_dwDetectTime = m_DetectTimeCombo.GetCurSel();
	m_nDetectTimerTime = (int)((m_Config.m_dwDetectTime + 1) * 1000);

	m_Config.SaveConfig();
}


HWND CMy9RASTOPDlg::GetWinHandle(ULONG pid, TCHAR *szClassName, TCHAR *szTitle) // 프로세스 아이디로 윈도우 핸들 얻기   
{
	// find all hWnds (vhWnds) associated with a process id (dwProcessID)
	HWND hDesktop = ::GetDesktopWindow();
	HWND hCurrWnd = ::FindWindowEx(hDesktop, NULL, szClassName, NULL );

	while (hCurrWnd != NULL) {
		DWORD dwProcessID = 0;
		GetWindowThreadProcessId(hCurrWnd, &dwProcessID);

		if (pid == dwProcessID)
		{
			TCHAR tmpTitle[512];
			::GetWindowText(hCurrWnd, tmpTitle, 512);

			if (szTitle == NULL)
				return hCurrWnd;
			else if(wcsstr(tmpTitle, szTitle) )
				return hCurrWnd;
		}

		hCurrWnd = ::FindWindowEx(hDesktop, hCurrWnd, szClassName, NULL);
	}

	return NULL;
}

void CMy9RASTOPDlg::AddBW(WPARAM bwType, DWORD dwPid) {

	// 크롬 [- Chrome], [Chrome_WidgetWin_1]
	// ie [- Internet Explorer], [IEFrame]
	// Edge [], [TabWindowClass]
	// firefox [- Mozilla Firefox], [MozillaWindowClass]
	HWND hWnd = NULL;

	switch (bwType) {
	case dBW_CHROME:
		hWnd = GetWinHandle(dwPid, _T("Chrome_WidgetWin_1"), _T("- Chrome"));
		break;
	case dBW_EDGE:
		hWnd = GetWinHandle(dwPid, _T("TabWindowClass"), NULL);
		break;
	case dBW_IE:
		hWnd = GetWinHandle(dwPid, _T("IEFrame"), _T("- Internet Explorer"));
		break;
	case dBW_FIREFOX:
		hWnd = GetWinHandle(dwPid, _T("MozillaWindowClass"), _T("Mozilla Firefox"));
		if( hWnd == NULL )
			hWnd = GetWinHandle(dwPid, _T("MozillaWindowClass"), _T("- Mozilla Firefox"));
		break;
	default:
		hWnd = NULL;
		break;
	}

	if (hWnd != NULL) {
		sPBW_DATA pData = (sPBW_DATA)malloc(sizeof(sBW_DATA));

		if (pData) {
			pData->m_hWnd = hWnd;
			pData->m_dwPid = dwPid;

			m_BwList.push(pData);

			UpdateBwCntStatic();
		}
	}
}

void				CMy9RASTOPDlg::UpdateBW() {

	HANDLE hSnapShot;
	PROCESSENTRY32 pEntry;

	// TH32CS_SNAPALL 옵션으로 모든 프로세스의 정보를 얻어냅니다.
	hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

	pEntry.dwSize = sizeof(pEntry);

	// Process32First 함수를 통해서 SnapShot에 들어 있는 첫번째 프로세스를 받아옵니다.
	Process32First(hSnapShot, &pEntry);

	while (true) {
		// Process32Next 함수를 통해서 pEntry 의 다음 프로세스를 스냅샷에서 받아 옵니다.
		BOOL hRes = Process32Next(hSnapShot, &pEntry);

		if (hRes == FALSE)
			break;

		DWORD dwType = dBW_NONE;

		if (!wcscmp(pEntry.szExeFile, _T("chrome.exe"))) {
			dwType = dBW_CHROME;
		}
		else if (!wcscmp(pEntry.szExeFile, _T("MicrosoftEdgeCP.exe"))) {
			dwType = dBW_EDGE;
		}
		else if (!wcscmp(pEntry.szExeFile, _T("firefox.exe"))) {
			dwType = dBW_FIREFOX;
		}
		else if (!wcscmp(pEntry.szExeFile, _T("iexplore.exe"))) {
			dwType = dBW_IE;
		}

		if (dwType != dBW_NONE) {
			if (!FindBwByPID(pEntry.th32ProcessID)) {
				AddBW(dwType, pEntry.th32ProcessID);
			}
		}
	}
}

BOOL				CMy9RASTOPDlg::FindBwByPID(DWORD dwPid) {
	sPLIST_BLOCK list, block, next_block;

	list = m_BwList.GetFirstBlock();

	LIST_WHILE(list, block, next_block, next);
	sPBW_DATA pData = (sPBW_DATA)block->data;

	if (pData->m_dwPid == dwPid)
		return TRUE;

	LIST_WHILEEND();

	return FALSE;
}

BOOL				CMy9RASTOPDlg::hasDetectSite(DWORD dwPid) {
	for (int i = 0; i < dMAX_SITE_CNT; i++) {
		if (m_DetectSitePid[i] == dwPid) {
			return TRUE;
		}
	}
	return FALSE;
}

void				CMy9RASTOPDlg::UpdateBwCntStatic() {
	TCHAR szMsg[512];
	wsprintf(szMsg, _T("감지대상 웹브라우저: %d 개"), m_BwList.GetSize());
	GetDlgItem(IDC_STATIC_BWCNT)->SetWindowText(szMsg);
}


// 모든 서비스 자동시작 끄기
void CMy9RASTOPDlg::OnBnClickedButton10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Title(_T("사이트 목록의 모든 서비스의 자동시작을 끄는중.."));

	int nSiteNum = 0;

	while (1) {
		sPSITE_INFO pSite = (sPSITE_INFO)&g_sSiteInfo[nSiteNum];

		if (pSite->m_szSiteName == NULL)
			break;

		int nProgNum = 0;


		while (1) {
			sPONE_PRC_INFO pPrc = (sPONE_PRC_INFO)&pSite->m_PrcInfo[nProgNum];

			if (pPrc->m_szCmdName == NULL)
				break;

			if (pPrc->m_szSvcName != NULL) {
				TCHAR szPara[512];
				wsprintf(szPara, _T("config \"%s\" start=demand"), pPrc->m_szSvcName);

				ShellExecute(NULL, _T("open"), _T("sc.exe"), szPara, NULL, SW_HIDE);
			}

			nProgNum++;
		}
			
		nSiteNum++;
	}

	Title(_T("사이트 목록의 모든 서비스의 자동시작을 수동으로 전환하였습니다."));
}


LRESULT CMy9RASTOPDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (message == WM_QUERYENDSESSION) {
		CDialog::OnOK();
		return TRUE;
	}

	return CDialogEx::DefWindowProc(message, wParam, lParam);
}
