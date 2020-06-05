
// DirectSeverDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "DirectServer.h"
#include "DirectServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_TRAY_NOTIFYICACTION WM_APP+10

enum {
	TRAY_BALLOON = 0
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


// CDirectSeverDlg 대화 상자



CDirectServerDlg::CDirectServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIRECTSEVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDirectServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDirectServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_TRAY_NOTIFYICACTION, OnTaryNotifyAction)
	ON_COMMAND(ID_TRAY_EXIT, &CDirectServerDlg::OnTrayExit)
	ON_COMMAND(ID_TRAY_OPEN, &CDirectServerDlg::OnTrayOpen)
END_MESSAGE_MAP()


// CDirectSeverDlg 메시지 처리기

BOOL CDirectServerDlg::OnInitDialog()
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
	//m_fn_Tray();
	m_fn_InitServer();
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CDirectServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == SC_CLOSE)
	{
		m_fn_Tray();
		m_fn_Ballon();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CDirectServerDlg::OnPaint()
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
HCURSOR CDirectServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDirectServerDlg::m_fn_WriteLog(char* strLog, ...)
{
	char chLog[LOG_TEXT_MAX_SIZE] = { 0, };
	char chBuff[LOG_TEXT_MAX_SIZE] = { 0, };
	wchar_t wchMsg[LOG_TEXT_MAX_SIZE] = { 0, };
	int nScrollCount = 0;
	va_list ap;
	va_start(ap, strLog);
	vsnprintf_s(chLog, LOG_TEXT_MAX_SIZE, strLog, ap);
	va_end(strLog);

	SYSTEMTIME systime;
	GetLocalTime(&systime);
	sprintf_s(chBuff, "[%02d:%02d:%02d:%03d] %s", systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds, chLog);

	_in_fn_Multi2Uni(chBuff, wchMsg);
	
	nScrollCount = ((CListBox*)GetDlgItem(IDC_LIST_MESSGE))->GetCount();
	if (nScrollCount > 200)
		((CListBox*)GetDlgItem(IDC_LIST_MESSGE))->DeleteString(0);
	((CListBox*)GetDlgItem(IDC_LIST_MESSGE))->AddString(wchMsg);
	((CListBox*)GetDlgItem(IDC_LIST_MESSGE))->SetScrollPos(SB_VERT, nScrollCount, TRUE);
	((CListBox*)GetDlgItem(IDC_LIST_MESSGE))->SetTopIndex(nScrollCount - 1);
}

void CDirectServerDlg::m_fn_WriteInfo(char* strLog, ...)
{
	char chLog[LOG_TEXT_MAX_SIZE] = { 0, };
	char chBuff[LOG_TEXT_MAX_SIZE] = { 0, };
	wchar_t wchMsg[LOG_TEXT_MAX_SIZE] = { 0, };
	int nScrollCount = 0;
	va_list ap;
	va_start(ap, strLog);
	vsnprintf_s(chLog, LOG_TEXT_MAX_SIZE, strLog, ap);
	va_end(strLog);

	_in_fn_Multi2Uni(chLog, wchMsg);
	
	nScrollCount = ((CListBox*)GetDlgItem(IDC_LIST_INFO))->GetCount();
	if (nScrollCount > 200)
		((CListBox*)GetDlgItem(IDC_LIST_INFO))->DeleteString(0);
	((CListBox*)GetDlgItem(IDC_LIST_INFO))->AddString(wchMsg);
	((CListBox*)GetDlgItem(IDC_LIST_INFO))->SetScrollPos(SB_VERT, nScrollCount, TRUE);
	((CListBox*)GetDlgItem(IDC_LIST_INFO))->SetTopIndex(nScrollCount - 1);
}

void CDirectServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_fn_DeleteTrayIcon();
	if (mc_pServerClass != nullptr)
		delete mc_pServerClass;
}

BOOL CDirectServerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SPACE)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDirectServerDlg::m_fn_InitServer()
{
	m_fn_WriteInfo("Server IP : 127.0.0.1");
	m_fn_WriteInfo("Server Port : 20000");

	if (mc_pServerClass != nullptr)
		delete mc_pServerClass;
	mc_pServerClass = new ServerClass();
	mc_pServerClass->RegPrintFunc(std::bind(&CDirectServerDlg::m_fn_WriteLog, this, std::placeholders::_1));
	if (!mc_pServerClass->IsConnected())
	{
		m_fn_WriteLog("FAILED to init server Socket.");
		m_fn_WriteLog("Exit");
		this->DestroyWindow();
	}

	mc_pServerClass->RunListen();

}

void CDirectServerDlg::m_fn_Tray()
{
	NOTIFYICONDATA nid;

	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.uID = 0;    // 트레이 구조체 아이디.
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.hWnd = m_hWnd;
	nid.hIcon = AfxGetApp()->LoadIconW(IDR_MAINFRAME);

	nid.uCallbackMessage = WM_TRAY_NOTIFYICACTION;
	lstrcpy(nid.szTip, _T("Tray Icon"));

	::Shell_NotifyIcon(NIM_ADD, &nid);
	//SetTimer(TRAY_BALLOON, 1000 * 60, NULL);
	AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE);
	this->ShowWindow(SW_HIDE);
}

LRESULT CDirectServerDlg::OnTaryNotifyAction(WPARAM wParam, LPARAM lParam)
{
	int nRst = 0;
	switch (lParam)
	{
	case WM_RBUTTONDOWN:
	{
		CPoint ptMouse;
		::GetCursorPos(&ptMouse);
		CMenu menu;
		menu.LoadMenu(IDR_TRAYMENU);
		CMenu *pMenu = menu.GetSubMenu(0); //활성화 할 메뉴 지정
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMouse.x, ptMouse.y, AfxGetMainWnd());
	}
	break;

	case WM_LBUTTONDBLCLK:
	{
		nRst = m_fn_DeleteTrayIcon();
		if (nRst == -1)
			return nRst;
		AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOW); //윈도우 활성화
		//KillTimer(TRAY_BALLOON);
	}
	break;
	}
	return 1;
}

int CDirectServerDlg::m_fn_DeleteTrayIcon()
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = GetSafeHwnd();

	BOOL bRet = ::Shell_NotifyIcon(NIM_DELETE, &nid); //트레이아이콘 제거
	if (!bRet)
	{
		//::AfxMessageBox(_T("트레이아이콘 제거실패"));
		return -1;
	}
	return 0;
}

void CDirectServerDlg::OnTrayExit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	DestroyWindow();
}

void CDirectServerDlg::OnTrayOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_fn_DeleteTrayIcon();
	AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOW); //윈도우 활성화
}

void CDirectServerDlg::m_fn_Ballon()
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.dwInfoFlags = NIIF_WARNING;        // 풍선 알림아이콘 종류
	nid.uFlags = NIF_MESSAGE | NIF_INFO | NIF_ICON;
	nid.uTimeout = 3000;
	nid.hWnd = AfxGetApp()->m_pMainWnd->m_hWnd;
	nid.uCallbackMessage = WM_TRAY_NOTIFYICACTION;
	nid.hIcon = AfxGetApp()->LoadIconW(IDR_MAINFRAME);

// 	lstrcpy(nid.szInfo, m_pTrayText);
// 	lstrcpy(nid.szInfoTitle, m_pTrayTitle);
	lstrcpy(nid.szInfo, _T("프로그램 활성화: 더블클릭, Tray우클릭-열기\n프로그램 종료: Tray우클릭-종료"));
	lstrcpy(nid.szInfoTitle, _T("Direct Server"));
	::Shell_NotifyIcon(NIM_MODIFY, &nid);
}