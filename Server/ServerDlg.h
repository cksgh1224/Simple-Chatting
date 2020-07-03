
// ServerDlg.h: 헤더 파일
//

#pragma once

#define MAX_CLIENT_COUNT 5 // 서버에 접속 가능한 클라이언트 수

// CServerDlg 대화 상자
class CServerDlg : public CDialogEx
{
private:
	SOCKET mh_listen_socket; // listen socket : 클라이언트의 접속을 받아주는 소켓

	SOCKET mh_client_list[MAX_CLIENT_COUNT]; // 클라이언트 배열
	wchar_t m_client_ip[MAX_CLIENT_COUNT][24]; // 클라이언트 IP 배열 (wchar_t: 유니코드를 위한 char)
	int m_client_count; // 현재 연결된 클라이언트 수

// 생성입니다.
public:
	CServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	void AcceptProcess(SOCKET parm_h_socket); // 클라이언트 접속 시도 처리
	void ClientCloseProcess(SOCKET parm_h_socket, char parm_force_flag); // 클라이언트의 접속 해제
	void ReceiveData(SOCKET parm_h_socket, char* p_body_data, unsigned short body_size); // recv (body 데이터 읽기)

	void AddEventString(CString parm_string); // 리스트 박스 메시지 추가

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
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
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	CListBox m_event_list;
};
