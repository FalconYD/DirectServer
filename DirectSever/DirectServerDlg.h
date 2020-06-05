
// DirectSeverDlg.h: 헤더 파일
//

#pragma once


// CDirectSeverDlg 대화 상자
class CDirectServerDlg : public CDialogEx
{
// 생성입니다.
public:
	CDirectServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIRECTSEVER_DIALOG };
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
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	ServerClass* mc_pServerClass;
	void m_fn_WriteLog(char* strLog, ...);
	void m_fn_InitServer();
	void m_fn_Tray();
	afx_msg LRESULT OnTaryNotifyAction(WPARAM wParam, LPARAM lParam);
	int m_fn_DeleteTrayIcon();
	afx_msg void OnTrayExit();
	afx_msg void OnTrayOpen();
	void CDirectServerDlg::m_fn_Ballon();
	void CDirectServerDlg::m_fn_WriteInfo(char* strLog, ...);
};
