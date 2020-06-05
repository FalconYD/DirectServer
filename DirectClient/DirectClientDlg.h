
// DirectClientDlg.h : 헤더 파일
//

#pragma once


// CDirectClientDlg 대화 상자
class CDirectClientDlg : public CDialogEx
{
// 생성입니다.
public:
	CDirectClientDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIRECTCLIENT_DIALOG };
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
