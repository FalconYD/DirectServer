
// DirectClientDlg.h : ��� ����
//

#pragma once


// CDirectClientDlg ��ȭ ����
class CDirectClientDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CDirectClientDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIRECTCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	void m_fn_InitUI();
	void m_fn_GetData();
	void m_fn_WriteLog(const char* strLog, ...);
	
	static void m_fn_Callback(CMDMSG stCmdMsg);
	CMDMSG m_stCmdMsg;
	afx_msg void OnBnClickedBnSend();
	THREAD_PARAM m_stThreadConnect;
	static unsigned int __stdcall THREAD_CONNECT(LPVOID pParam);
	void m_fn_RunConnet();
	void m_fn_EnableControl(bool bEnable);
};
