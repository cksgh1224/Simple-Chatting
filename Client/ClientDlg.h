
// ClientDlg.h: 헤더 파일
//

#pragma once

#define NM_CHAT_DATA      1   // 채팅 데이터의 message_id 는 1이다


class CClientDlg;

class MyClient : public ClientSocket
{
private:
	CClientDlg* mp_parent;

public:
	MyClient(CClientDlg* ap_parent) : ClientSocket(0x27)
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

// 생성입니다.
public:
	CClientDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.


	// 리스트 박스 메시지 추가
	void AddEventString(CString parm_string); 
	void AddEventString(const wchar_t* ap_string); 


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
};
