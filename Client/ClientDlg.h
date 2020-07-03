
// ClientDlg.h: 헤더 파일
//

#pragma once


// CClientDlg 대화 상자
class CClientDlg : public CDialogEx
{
private:
	SOCKET mh_socket; // 서버에 접속할 소켓 핸들

// 생성입니다.
public:
	CClientDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	void AddEventString(CString parm_string); // 리스트 박스 메시지 추가

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CListBox m_event_list;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
