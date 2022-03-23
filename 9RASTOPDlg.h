
// 9RASTOPDlg.h: 헤더 파일
//

#pragma once


#include "ConfigMgr.h"
#include "WmiNoti.h"

#define dMAX_PROCS				512
#define dMAX_SITE_CNT			512

typedef struct {
	HWND		m_hWnd;
	DWORD		m_dwPid;
} sBW_DATA, *sPBW_DATA;


typedef struct {
	int			m_nSiteNum;
	DWORD		m_dwPid;
	HWND		m_hWnd;
} sDT_DATA, *sPDT_DATA;

// CMy9RASTOPDlg 대화 상자
class CMy9RASTOPDlg : public CDialogEx
{
// 생성입니다.
public:
	CMy9RASTOPDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MY9RASTOP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	
	ConfigMgr		m_Config;

	CListCtrl m_ProcList;

	void	InitProcList();
	void	UpdateProcList();
	void	UpdateProcList2();

	BOOL	findProcs(TCHAR *szProcsName);
	int     getServiceCnt(int nSiteIndex);
	int     getProgCnt(int nSiteIndex);
	int		getInstCnt(int nSiteIndex);

	void	UpdateProcessInfo();
	void	initProcs();
	TCHAR	m_szProcs[dMAX_PROCS][MAX_PATH];
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();

	void	Exec(TCHAR *cmd, TCHAR *param, TCHAR *dir, BOOL bWait, BOOL bShow = FALSE );

	int		ServiceStart(TCHAR * serviceName);
	int		ServiceStop(TCHAR * serviceName);
	int		SafeServiceStop(TCHAR * serviceName);
	DWORD	getServiceState(TCHAR * serviceName);
	BOOL	isExistService(TCHAR * serviceName);

	int		m_nUpdateCnt;
	void	StartUpdateTimer();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	int		PumpMessage();

	void	Title(TCHAR *szMsg);

	BOOL	makeBitDir(TCHAR *src, TCHAR *dir32, TCHAR *dir64);

	void	waitProcess(TCHAR *szProcName);


	void	CreateTray();
	void	DestroyTray();
	afx_msg void OnDestroy();

	afx_msg LRESULT OnMainTray(WPARAM wParam, LPARAM lParam);
	UINT	m_uShellRestart;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnOpenDlg();
	afx_msg void OnExitMplug();

	afx_msg void OnBnClickedButton5();

public:
	int		m_nMinSiteNameLen;

public:

	BOOL	m_bWithInst;
	BOOL	m_bWinStarExe;
	BOOL	m_bStartWithAllStop;
	BOOL	m_bSiteAutoDetect;

public:

	void		DisableAllBtn() {
		GetDlgItem(IDC_BUTTON3)->EnableWindow(0);		// 전체선택
		GetDlgItem(IDC_BUTTON4)->EnableWindow(0);		// 전체해제
		GetDlgItem(IDC_BUTTON1)->EnableWindow(0);		// 정지
		GetDlgItem(IDC_BUTTON2)->EnableWindow(0);		// 시작
		GetDlgItem(IDC_BUTTON5)->EnableWindow(0);		// 프로그램 종료 
		GetDlgItem(IDC_BUTTON6)->EnableWindow(0);		// 프로그램 종료 
	}

	void		EnableAllBtn() {
		GetDlgItem(IDC_BUTTON3)->EnableWindow(1);		// 전체선택
		GetDlgItem(IDC_BUTTON4)->EnableWindow(1);		// 전체해제
		GetDlgItem(IDC_BUTTON1)->EnableWindow(1);		// 정지
		GetDlgItem(IDC_BUTTON2)->EnableWindow(1);		// 시작
		GetDlgItem(IDC_BUTTON5)->EnableWindow(1);		// 프로그램 종료 
		GetDlgItem(IDC_BUTTON6)->EnableWindow(1);		// 프로그램 종료 
	}
	afx_msg void OnClickedWinStartRun();
	afx_msg void OnClickedAllStopRun();
	afx_msg void OnClickedSiteAutoDetect();
	afx_msg void OnClickedWithInst();

	void	AllStopAsRun();
	afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);

	void	DetectSiteProc();

public:
	void	StopProcess(BOOL bIsTimer);
	void	StartProcess(BOOL bIsTimer);

	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();

public:
	BOOL			RegistStartUp(BOOL is64Bit);
	BOOL			UnRegistStartUp(BOOL is64Bit);

	BOOL			m_bHideAsRun;
	afx_msg void	OnClickedCheck5();
	afx_msg void	OnBnClickedButton8();
	afx_msg void	OnBnClickedButton9();
	CComboBox		m_DetectTimeCombo;

	int				m_nDetectTimerTime;
	afx_msg void	OnCbnSelchangeCombo1();

public:
	CWmiNoti			m_WmiNoti;
	MYLIST::CList		m_BwList;

	HWND				GetWinHandle(ULONG pid, TCHAR *szClassName, TCHAR *szTitle );		 // 프로세스 아이디로 윈도우 핸들 얻기   

	void				AddBW(WPARAM bwType, DWORD dwPid);
	int					FindSite(TCHAR *szBwTitle);

	void				UpdateBW();
	BOOL				FindBwByPID(DWORD dwPid);
	BOOL				m_bNeedBwUpdate;

	DWORD				m_DetectSitePid[dMAX_SITE_CNT];
	BOOL				hasDetectSite(DWORD dwPid);

	void				UpdateBwCntStatic();
	afx_msg void OnBnClickedButton10();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
