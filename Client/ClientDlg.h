
// ClientDlg.h: 헤더 파일
//

#pragma once

#define NM_CHAT_DATA      1   // 채팅 데이터   Message ID
#define NM_LOGIN_DATA     2   // 로그인 데이터 Message ID

class CClientDlg;

class MyClient : public ClientSocket
{
private:
	CClientDlg* mp_parent; // 대화상자 객체의 주소를 저장할 변수

public:
	// CClientDlg 대화상자의 리스트 박스 를 사용하기 위해 객체를 생성할 때 대화상자의 주소를 넘겨준다
	MyClient(CClientDlg* ap_parent) : ClientSocket(0x27) // 프로토콜 헤더의 시작 1바이트를 0x27로 설정 (구분값)
	{
		mp_parent = ap_parent;
	}
	
	// 수신된 데이터를 처리하는 함수
	virtual int ProcessRecvData(SOCKET ah_socket, unsigned char a_msg_id, char* ap_recv_data, BS a_body_size);
};


// CClientDlg 대화 상자
class CClientDlg : public CDialogEx
{
private:
	MyClient m_client;

	UserData user_data; // 로그인 정보를 담고있는 객체

// 생성입니다.
public:
	CClientDlg(UserData a_user, CWnd* pParent = nullptr);	// CClientDlg 생성자에 m_client(this) 추가

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
public:
	afx_msg void OnBnClickedSendBtn();
protected:
	afx_msg LRESULT OnConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReadAndClose(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedConnectBtn();
	afx_msg void OnBnClickedDisconnectBtn();
};
