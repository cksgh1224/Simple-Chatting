
// ServerDlg.h: 헤더 파일
//

#pragma once

#define MAX_CLIENT_COUNT 50 // 서버에 접속 가능한 클라이언트 수
#define NM_CHAT_DATA      1 // 채팅 데이터의 message_id 는 1이다

class CServerDlg; // 클래스 전방선언

class MyServer : public ServerSocket
{
private:
	CServerDlg* mp_parent; // CServerDlg 대화상자를 가리키는 포인터 (리스트 박스 작업을 할 때 사용)

public:
	// CServerDlg 대화상자의 m_event_list(리스트 박스) 를 사용하기 위해 객체를 생성할 때 대화상자의 주소를 넘겨준다
	// CServerDlg 생성자에 m_server(this)
	MyServer(CServerDlg* ap_parent) : ServerSocket(0x27, MAX_CLIENT_COUNT, new UserData) 
	{
		// 0x27: 구분값(16진수), 최대 사용자 수, 사용자를 관리하기 위한 클래스(UserData / UserAccount - 로그인)
		mp_parent = ap_parent;
	}
	

	// Accept 시에 추가적으로 해야할 작업 처리
	virtual void AddWorkForAccept(UserData* ap_user);

	// 최대 사용자수 초과시에 추가적으로 해야할 작업 처리
	virtual void ShowLimitError(const wchar_t* ap_ip_address);

	// 클라이언트 접속 해제시에 추가적으로 해야할 작업 처리
	virtual void AddWorkForCloseUser(UserData* ap_user, int a_error_code);

	// 수신된 데이터를 처리하는 함수
	virtual int ProcessRecvData(SOCKET ah_socket, unsigned char a_msg_id, char* ap_recv_data, BS a_body_size);
};


class MyServer;

// CServerDlg 대화 상자
class CServerDlg : public CDialogEx
{
private:
	MyServer m_server;

private:

	//SOCKET mh_listen_socket; // listen socket : 클라이언트의 접속을 받아주는 소켓

	//SOCKET mh_client_list[MAX_CLIENT_COUNT]; // 클라이언트 배열
	//wchar_t m_client_ip[MAX_CLIENT_COUNT][24]; // 클라이언트 IP 배열 (wchar_t: 유니코드를 위한 char)
	//int m_client_count; // 현재 연결된 클라이언트 수

// 생성입니다.
public:
	CServerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	
	void AddEventString(CString parm_string); // 리스트 박스 메시지 추가
	void AddEventString(const wchar_t* ap_string);

	//void AcceptProcess(SOCKET parm_h_socket); // 클라이언트 접속 시도 처리
	//void ClientCloseProcess(SOCKET parm_h_socket, char parm_force_flag); // 클라이언트 접속 해제

	//void ReadFrameData(const SOCKET& h_socket); // 데이터 읽기 (헤더 + 바디)
	//void ReceiveData(SOCKET parm_h_socket, char* p_body_data, unsigned short body_size); // body 데이터 읽기

	//void SendFrameData(SOCKET parm_h_socket, unsigned char parm_id, const void* parm_p_data, int parm_size); // 데이터 전송 (send)


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
	//virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	CListBox m_event_list;
};
