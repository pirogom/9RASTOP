
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define USE_PROCESS_DETECT

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.


#include <afxdisp.h>        // MFC 자동화 클래스입니다.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원
#include <afxcontrolbars.h>

#include "clist.h"

#define dPRC_IS_SERVICE			0
#define dPRC_IS_CONSOLE			1

#define WM_MAIN_TRAY			(WM_USER+1)

#define dMPLUG_EXE_NAME			"mplug.exe"

/**
* List Control Defines
**/
#define ON_LIST_CHECK		0x2000 
#define OFF_LIST_CHECK		0x1000 

#define LIST_CHECK_STATE( list, item ) ListView_GetItemState( list.m_hWnd, item, LVIS_STATEIMAGEMASK );
#define IS_CHECKED( list, item ) ( ListView_GetItemState( list.m_hWnd, item, LVIS_STATEIMAGEMASK ) == 0x2000 )
#define IS_UNCHECKED( list, item ) ( ListView_GetItemState( list.m_hWnd, item, LVIS_STATEIMAGEMASK ) == 0x1000 )
#define ON_CHECKED( list, item ) ListView_SetItemState( list.m_hWnd, item, 0x2000, LVIS_STATEIMAGEMASK);
#define OFF_CHECKED( list, item ) ListView_SetItemState( list.m_hWnd, item, 0x1000, LVIS_STATEIMAGEMASK);

#define INIT_LVITEM( listctl ) 					\
	LVITEM lvitem;								\
	TCHAR szLvItemBuf[256];						\
	int nItemCount = listctl.GetItemCount();	\
	int nSubItem = 0;							

#define SET_LVITEM( listctl, buf, ... )		\
	lvitem.mask = LVIF_TEXT;					\
	lvitem.iItem = nItemCount;					\
	lvitem.iSubItem = nSubItem++;				\
	wsprintf( szLvItemBuf, buf, __VA_ARGS__ );	\
	lvitem.pszText = szLvItemBuf;				\
	if( nSubItem == 1 ){						\
		listctl.InsertItem( &lvitem );			\
		} else	{									\
		listctl.SetItem( &lvitem );				\
		}


#define UPDATE_LVITEM( listctl, itemnum, subnum, buf, ... ) \
			lvitem.mask = LVIF_TEXT;	\
			lvitem.iItem = itemnum;		\
			lvitem.iSubItem = subnum;\
			wsprintf( szLvItemBuf, buf, __VA_ARGS__ );			\
			lvitem.pszText = szLvItemBuf;				\
			listctl.SetItem( &lvitem ); 

//
typedef struct s_list_config
{
	TCHAR		*m_szTitle;
	int			m_nWidth;
	int			m_nFmt;
} sITEM_CONFIG, *sPLIST_CONFIG;

#define WM_NEW_BROWSER		WM_USER+1

#define dBW_NONE			0
#define dBW_CHROME			1
#define dBW_EDGE			2
#define dBW_IE				3
#define dBW_FIREFOX			4

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


