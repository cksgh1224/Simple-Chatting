
// ServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// MyServer 클래스 메서드들

// Accept 시에 추가적으로 해야할 작업 처리
// 클라이언트가 connect 함수를 호출하여 서버에 접속을 시도하면 서버에서 accept를 이용해 클라이언트의 접속을 받아준다
// accept 이후 클라이언트에게 하고싶은 행위가 있으면 이 함수에서 처리
// 매개변수로 넘어오는 UserData* ap_user는 현재 접속한 사용자(클라이언트)의 정보를 담고있다
void MyServer::AddWorkForAccept(UserData* ap_user)
{
	CString str;
	str.Format(L"%s 에서 사용자가 접속했습니다!", ap_user->GetIP());
	mp_parent->AddEventString(str);
}


// 최대 사용자수 초과시에 추가적으로 해야할 작업 처리
void MyServer::ShowLimitError(const wchar_t* ap_ip_address)
{
	CString str;
	str.Format(L"최대 접속인원 초과!");
	mp_parent->AddEventString(str);
}


// 클라이언트 접속 해제시에 추가적으로 해야할 작업 처리
// 매개변수로 넘어오는 UserData* ap_user는 접속을 해제하는 사용자(클라이언트)의 정보를 담고있다
// a_error_code : 0이면 정상종료, -1이면 키값이 유효하지 않아서 종료, -2이면 바디정보 수신중에 오류 발생
void MyServer::AddWorkForCloseUser(UserData* ap_user, int a_error_code)
{
	CString str;
	str.Format(L"%s 에서 사용자가 접속을 해제했습니다!", ap_user->GetIP());
	mp_parent->AddEventString(str);
}


// 수신된 데이터를 처리하는 함수
// ah_socket    : 데이터를 전송한 클라이언트 소켓의 핸들, a_msg_id: 클라이언트가 보낸 데이터가 어떤 종류의 데이터인지
// ap_recv_data : 클라이언트가 보낸 데엍, a_body_size: 클라이언트가 보낸 데이터의 body size (헤더를 뺀 바디 크기)
int MyServer::ProcessRecvData(SOCKET ah_socket, unsigned char a_msg_id, char* ap_recv_data, BS a_body_size)
{
	// 대용량 데이터가 전송 또는 수신될 때, 필요한 기본 코드를 수행
	ServerSocket::ProcessRecvData(ah_socket, a_msg_id, ap_recv_data, a_body_size);
	
	
	UserData* p_user = ServerSocket::FindUserData(ah_socket); // 데이터를 보낸 클라이언트가 어떤 것인지 찾는다
	
	if (a_msg_id == NM_CHAT_DATA) // 수신된 데이터가 채팅 데이터인 경우
	{
		CString str;
		str.Format(L"%s : %s", p_user->GetIP(), (wchar_t*)ap_recv_data); // (wchar_t*)ap_recv_data 유니코드로 형변환
		mp_parent->AddEventString(str);
		
		// 서버에 연결된 모든 클라이언트들에게 수신된 데이터를 전송
		for (int i = 0; i < MAX_CLIENT_COUNT; i++)
		{
			// 현재 사용자가 접속 상태인지 확인한다
			if (mp_user_list[i]->GetHandle() != INVALID_SOCKET)
			{
				// 데이터 전송
				SendFrameData(mp_user_list[i]->GetHandle(), NM_CHAT_DATA, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);
				// 문자열을 전송할 때는 NULL 문자를 포함해서 전송하기 때문에 문자열 길이에 1을 더하고 
				// 유니코드에서 문자 1개는 2byte를 차지하기 때문에 문자열 길이에 2를 곱한다
			}
		}

	}

	return 1; // 정상적으로 데이터를 받았으면 1반환 
	// 잘못된 사용자가 메시지를 보냈을때는 소켓을 끊고 0을 반환
}









// CServerDlg 대화 상자


CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent), m_server(this) // m_server(this): 객체를 생성할 때 대화상자의 주소를 넘겨준다
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_event_list);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(25001, &CServerDlg::OnAcceptUser)
	ON_MESSAGE(25002, &CServerDlg::OnReadAndClose)
	ON_BN_CLICKED(IDC_START_BTN, &CServerDlg::OnBnClickedStartBtn)
	ON_BN_CLICKED(IDC_SEND_BTN, &CServerDlg::OnBnClickedSendBtn)
	ON_BN_CLICKED(IDC_USER_BTN, &CServerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// 리스트 박스 메시지 추가
void CServerDlg::AddEventString(CString parm_string)
{
	int index = m_event_list.InsertString(-1, parm_string); // 리스트 목록 끝에(-1) 문자열(parm_string) 추가. 반환값(index): 추가되는 위치
	m_event_list.SetCurSel(index);                          // 추가한 곳(index) 커서 활성화
}


// CServerDlg 메시지 처리기

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.


	// 서버 서비스를 '192.168.77.100'에서 27100 포트로 시작한다 (socket - bind - listen)
	//m_server.StartServer(L"192.168.77.100", 27100, m_hWnd);
	//m_server.StartServer(L"127.0.0.1", 27100, m_hWnd);
	//AddEventString(L"서버 서비스를 시작합니다!");



	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// Enter 종료 막기
BOOL CServerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN) return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}




// 새로운 클라이언트가 접속했을 때 발생하는 메시지를 처리한다
// FD_ACCEPT : 25001 메시지
afx_msg LRESULT CServerDlg::OnAcceptUser(WPARAM wParam, LPARAM lParam)
{
	// ProcessToAccept: 클라이언트의 접속 처리(FD_ACCEPT 처리)
	m_server.ProcessToAccept(wParam, lParam);
	return 0;
}


// 접속한 클라이언트가 데이터를 전송하거나 접속을 해제할 때 발생하는 메시지를 처리한다
// FD_READ, FD_CLOSE : 25002 메시지
afx_msg LRESULT CServerDlg::OnReadAndClose(WPARAM wParam, LPARAM lParam)
{
	// ProcessClientEvent : 클라이언트의 네트워크 이벤트 처리 (FD_READ, FD_CLOSE 처리 함수)
	m_server.ProcessClientEvent(wParam, lParam);
	return 0;
}






// '시작' 버튼 클릭
void CServerDlg::OnBnClickedStartBtn()
{
	CString ip, port;
	GetDlgItemText(IDC_IP_EDIT, ip);
	GetDlgItemText(IDC_PORT_EDIT, port);

	if (ip.IsEmpty() || port.IsEmpty())
	{
		MessageBox(L"IP 주소, 포트번호를 입력해 주세요!", NULL, MB_OK);
		return;
	}

	m_server.StartServer(ip, _ttoi(port), m_hWnd); // _ttoi : cstring -> int
	AddEventString(L"서버 서비스를 시작합니다!");

	GetDlgItem(IDC_START_BTN)->EnableWindow(FALSE);
}


// '전송' 버튼 클릭
void CServerDlg::OnBnClickedSendBtn()
{
	UserData** user_list = m_server.GetUserList(); // 서버에 접속한 전체 사용자에 대한 정보
	
	CString str, text;
	GetDlgItemText(IDC_CHAT_EDIT, text);
	str.Format(L"%s : %s", L"Server", text);

	for (int i = 0; i < MAX_CLIENT_COUNT; i++)
	{
		// 현재 사용자가 접속 상태인지 확인한다
		if (user_list[i]->GetHandle() != INVALID_SOCKET)
		{
			m_server.SendFrameData(user_list[i]->GetHandle(), NM_CHAT_DATA, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2); // 데이터 전송
		}
	}

	AddEventString(str);
}





// '사용자 목록' 버튼 클릭
void CServerDlg::OnBnClickedButton1()
{
	UserData** user_list = m_server.GetUserList(); // 서버에 접속한 전체 사용자에 대한 정보
	CString str;
	
	for (int i = 0; i < MAX_CLIENT_COUNT; i++)
	{
		// 현재 사용자가 접속 상태인지 확인한다
		if (user_list[i]->GetHandle() != INVALID_SOCKET)
		{
			str += user_list[i]->GetIP();
			str += L"\n";
		}
	}

	MessageBox(str, L"사용자 목록", MB_OK);

}
