
// DirectClientDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "DirectClient.h"
#include "DirectClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDirectClientDlg* gc_pMainDlg;
// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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

// CDirectClientDlg ��ȭ ����

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

// CDirectClientDlg �޽��� ó����

BOOL CDirectClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_fn_InitUI();
	//_lib_Connect("127.0.0.1", 20000, m_fn_Callback);
	m_fn_RunConnet();
	
	
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CDirectClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CDirectClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDirectClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	
}

BOOL CDirectClientDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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