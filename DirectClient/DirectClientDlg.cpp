
// DirectClientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "DirectClient.h"
#include "DirectClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDirectClientDlg* gc_pMainDlg;
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

// CDirectClientDlg 대화 상자

CDirectClientDlg::CDirectClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIRECTCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	gc_pMainDlg = this;
}

void CDirectClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDirectClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BN_SEND, &CDirectClientDlg::OnBnClickedBnSend)
END_MESSAGE_MAP()

// CDirectClientDlg 메시지 처리기

BOOL CDirectClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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
	m_fn_InitUI();
	//_lib_Connect("127.0.0.1", 20000, m_fn_Callback);
	m_fn_RunConnet();
	
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CDirectClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDirectClientDlg::OnPaint()
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
HCURSOR CDirectClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDirectClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
}

BOOL CDirectClientDlg::PreTranslateMessage(MSG* pMsg)
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

void CDirectClientDlg::m_fn_InitUI()
{
	((CComboBox*)GetDlgItem(IDC_CB_MSGTYPE))->AddString(_T("BROADCAST"));
	((CComboBox*)GetDlgItem(IDC_CB_MSGTYPE))->AddString(_T("TARGET"));
	((CComboBox*)GetDlgItem(IDC_CB_MSGTYPE))->SetCurSel(0);

	GetDlgItem(IDC_EDIT_TASKSRC)->SetWindowText(_T("0"));
	GetDlgItem(IDC_EDIT_TASKDST)->SetWindowText(_T("0"));

	GetDlgItem(IDC_EDIT_FUNCSRC)->SetWindowText(_T("0"));
	GetDlgItem(IDC_EDIT_SEQSRC)->SetWindowText(_T("0"));
	GetDlgItem(IDC_EDIT_UNITSRC)->SetWindowText(_T("0"));

	GetDlgItem(IDC_EDIT_FUNCDST)->SetWindowText(_T("0"));
	GetDlgItem(IDC_EDIT_SEQDST)->SetWindowText(_T("0"));
	GetDlgItem(IDC_EDIT_UNITDST)->SetWindowText(_T("0"));
}

void CDirectClientDlg::m_fn_GetData()
{
	CString strTemp;
	m_stCmdMsg.uMsg_Type = ((CComboBox*)GetDlgItem(IDC_CB_MSGTYPE))->GetCurSel();
	((CEdit*)GetDlgItem(IDC_EDIT_TASKSRC))->GetWindowText(strTemp);
	m_stCmdMsg.uTask_Src = _ttoi(strTemp);
	((CEdit*)GetDlgItem(IDC_EDIT_TASKDST))->GetWindowText(strTemp);
	m_stCmdMsg.uTask_Dst = _ttoi(strTemp);

	((CEdit*)GetDlgItem(IDC_EDIT_FUNCSRC))->GetWindowText(strTemp);
	m_stCmdMsg.uFunID_Src = _ttoi(strTemp);
	((CEdit*)GetDlgItem(IDC_EDIT_SEQSRC))->GetWindowText(strTemp);
	m_stCmdMsg.uSeqID_Src = _ttoi(strTemp);
	((CEdit*)GetDlgItem(IDC_EDIT_UNITSRC))->GetWindowText(strTemp);
	m_stCmdMsg.uUnitID_Src = _ttoi(strTemp);

	((CEdit*)GetDlgItem(IDC_EDIT_FUNCDST))->GetWindowText(strTemp);
	m_stCmdMsg.uFunID_Dst = _ttoi(strTemp);
	((CEdit*)GetDlgItem(IDC_EDIT_SEQDST))->GetWindowText(strTemp);
	m_stCmdMsg.uSeqID_Dst = _ttoi(strTemp);
	((CEdit*)GetDlgItem(IDC_EDIT_UNITDST))->GetWindowText(strTemp);
	m_stCmdMsg.uUnitID_Dst = _ttoi(strTemp);

	((CEdit*)GetDlgItem(IDC_EDIT_INPUT))->GetWindowText(strTemp);
	m_stCmdMsg.uMsgSize = strTemp.GetLength();

	_in_fn_Uni2Multi(strTemp, (char*)m_stCmdMsg.cMsgBuf);
	_lib_SendData(m_stCmdMsg);
}

void CDirectClientDlg::OnBnClickedBnSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fn_GetData();
}

void CDirectClientDlg::m_fn_Callback(CMDMSG stCmdMsg)
{
	char strMsg[LOG_TEXT_MAX_SIZE] = { 0, };
	sprintf_s(strMsg, "Msg : %s", stCmdMsg.cMsgBuf);
	gc_pMainDlg->m_fn_WriteLog(strMsg);

	switch (stCmdMsg.uFunID_Dst)
	{
	case 0:
		break;
	default:
		break;
	}
}

void CDirectClientDlg::m_fn_WriteLog(const char* strLog, ...)
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

	_in_fn_Multi2Uni(chLog, wchMsg);

	nScrollCount = ((CListBox*)GetDlgItem(IDC_LIST_LOG))->GetCount();
	if (nScrollCount > 200)
		((CListBox*)GetDlgItem(IDC_LIST_LOG))->DeleteString(0);
	((CListBox*)GetDlgItem(IDC_LIST_LOG))->AddString(wchMsg);
	((CListBox*)GetDlgItem(IDC_LIST_LOG))->SetScrollPos(SB_VERT, nScrollCount, TRUE);
	((CListBox*)GetDlgItem(IDC_LIST_LOG))->SetTopIndex(nScrollCount - 1);
}
void CDirectClientDlg::m_fn_RunConnet()
{
	if (m_stThreadConnect.enState != THREAD_STATUS::THREAD_STAT_RUNNING && m_stThreadConnect.enState != THREAD_STATUS::THREAD_STAT_ACTIVE)
	{
		m_stThreadConnect.bThreadFlag = true;
		m_stThreadConnect.cObjectPointer1 = this;
		m_stThreadConnect.nScanInterval = 1000;
		m_stThreadConnect.enState = THREAD_STATUS::THREAD_STAT_NONE;
		HANDLE hThread = (HANDLE*)_beginthreadex(NULL, 0, THREAD_CONNECT, &m_stThreadConnect, 0, NULL);
		m_stThreadConnect.hThread = hThread;
		CloseHandle(hThread);
	}
}
unsigned int CDirectClientDlg::THREAD_CONNECT(LPVOID pParam)
{
	THREAD_PARAM* pThreadparam = (THREAD_PARAM*)pParam;
	CDirectClientDlg* pDlg = (CDirectClientDlg*)pThreadparam->cObjectPointer1;
	int nRet = -1;
	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_ACTIVE;
	while (pThreadparam->bThreadFlag)
	{
		Sleep(pThreadparam->nScanInterval);
		pThreadparam->enState = THREAD_STATUS::THREAD_STAT_RUNNING;

		nRet = _lib_Connect("127.0.0.1", 20000, m_fn_Callback);

		switch (nRet)
		{
		case -1: // err
			pDlg->m_fn_EnableControl(false);
			break;
		case 0: // conn
		case 1: // already conn
			pDlg->m_fn_EnableControl(true);
			break;
		}
	}
	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_COMPLETE;
	return 0;
}

void CDirectClientDlg::m_fn_EnableControl(bool bEnable)
{
	GetDlgItem(IDC_BN_SEND)->EnableWindow(bEnable);
	GetDlgItem(IDC_CB_MSGTYPE)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_INPUT)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_TASKSRC)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_TASKDST)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_FUNCSRC)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_SEQSRC)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_UNITSRC)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_FUNCDST)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_SEQDST)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_UNITDST)->EnableWindow(bEnable);
}