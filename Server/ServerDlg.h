
// ServerDlg.h: 헤더 파일
//

#pragma once

#define MAX_CLIENT_COUNT  3   // 서버에 접속 가능한 클라이언트 수

#define NM_CHAT_DATA      1   // 채팅 데이터   Message ID
#define NM_LOGIN_DATA     2   // 로그인 데이터 Message ID

class CServerDlg; // 전방선언

class MyServer : public ServerSocket
{
private:
	CServerDlg* mp_parent; // 대화상자 객체의 주소를 저장할 변수 (리스트 박스 작업을 할 때 사용)
	
	int user_count;        // 접속한 사용자 수

public:
	// CServerDlg 대화상자의 m_event_list(리스트 박스) 를 사용하기 위해 객체를 생성할 때 대화상자의 주소를 넘겨준다
	MyServer(CServerDlg* ap_parent) : ServerSocket(0x27, MAX_CLIENT_COUNT, new UserData)
	{
		// 프로토콜 구분값 27, 최대 사용자 수 50명, 사용자 관리용 클래스는 UserData
		mp_parent = ap_parent;
		user_count = 0;
	}
	

	// Accept 시에 추가적으로 해야할 작업 처리
	virtual void AddWorkForAccept(UserData* ap_user);

	// 최대 사용자수 초과 시에 추가적으로 해야할 작업 처리 (미구현)
	virtual void ShowLimitError(const wchar_t* ap_ip_address);

	// 클라이언트 접속 해제 시에 추가적으로 해야할 작업 처리
	virtual void AddWorkForCloseUser(UserData* ap_user, int a_error_code);

	// 수신된 데이터를 처리하는 함수
	virtual int ProcessRecvData(SOCKET ah_socket, unsigned char a_msg_id, char* ap_recv_data, BS a_body_size);


	int inline GetUserCount() { return user_count; } // 서버에 접속한 사용자 수 
};


// CServerDlg 대화 상자
class CServerDlg : public CDialogEx
{
private:
	MyServer m_server;

	UserData** dlg_user_list;  // 서버에 접속한 전체 사용자에 대한 정보


// 생성입니다.
public:
	CServerDlg(CWnd* pParent = nullptr);	// CServerDlg 생성자에 m_server(this) 추가
	virtual ~CServerDlg();

	void AddEventString(CString parm_string); // m_event_list 메시지 추가 (채팅 메시지)

	void AddUserString(CString parm_string);  // m_user_list 메시지 추가 (사용자 목록)

	void ResetUserString();                   // m_user_list 목록 초기화
	
	void SetUserList(UserData** a_user_list) { dlg_user_list = a_user_list; }


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
private:
	CListBox m_event_list; // 채팅 리스트 박스
	CListBox m_user_list;  // 사용자 리스트 박스
protected:
	
	afx_msg LRESULT OnAcceptUser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReadAndClose(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedStartBtn();
	afx_msg void OnBnClickedSendBtn();
	afx_msg void OnBnClickedDisconnectBtn();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedSend2Btn();
	afx_msg void OnBnClickedUserdataBtn();
};
