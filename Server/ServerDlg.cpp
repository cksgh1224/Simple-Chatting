
// ServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"

#include <string.h> // strtok 사용 (문자열 파싱)

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
	TR("MyServer::AddWorkForAccept - Accept 시에 추가적으로 해야할 작업 처리\n");
	
	user_count++;
}


// 최대 사용자수 초과시에 추가적으로 해야할 작업 처리
void MyServer::ShowLimitError(const wchar_t* ap_ip_address)
{
	TR("MyServer::ShowLimitError - 최대 사용자수 초과시에 추가적으로 해야할 작업 처리\n");
	
	CString str;
	str.Format(L"최대 접속인원 초과!");
	mp_parent->AddEventString(str);
}


// 클라이언트 접속 해제시에 추가적으로 해야할 작업 처리
// 매개변수로 넘어오는 UserData* ap_user는 접속을 해제하는 사용자(클라이언트)의 정보를 담고있다
// a_error_code : 0이면 정상종료, -1이면 키값이 유효하지 않아서 종료, -2이면 바디정보 수신중에 오류 발생
void MyServer::AddWorkForCloseUser(UserData* ap_user, int a_error_code)
{
	TR("MyServer::AddWorkForCloseUser - 클라이언트 접속 해제시에 추가적으로 해야할 작업 처리\n");
	// CloseSocket을 하기 전 (소켓 핸들이 아직 살아있는 상태)
	
	CString str;
	str.Format(L"%s 님이 접속을 해제했습니다 (%s)", ap_user->GetID(), ap_user->GetIP());
	mp_parent->AddEventString(str);
	


	mp_parent->ResetUserString();
	for (int i = 0; i < MAX_CLIENT_COUNT; i++)
	{	
		if (mp_user_list[i]->GetHandle() != INVALID_SOCKET)
		{
			CString str;
			str.Format(L"%s (%s)", mp_user_list[i]->GetID(), mp_user_list[i]->GetIP());
			mp_parent->AddUserString(str);
		}
	}

	user_count--;

	mp_parent->SetUserList(GetUserList());
}


// 수신된 데이터를 처리하는 함수
// ah_socket    : 데이터를 전송한 클라이언트 소켓의 핸들, a_msg_id: 클라이언트가 보낸 데이터가 어떤 종류의 데이터인지
// ap_recv_data : 클라이언트가 보낸 데엍, a_body_size: 클라이언트가 보낸 데이터의 body size (헤더를 뺀 바디 크기)
int MyServer::ProcessRecvData(SOCKET ah_socket, unsigned char a_msg_id, char* ap_recv_data, BS a_body_size)
{
	TR("MyServer::ProcessRecvData - 수신된 데이터를 처리하는 함수\n");
	
	// 대용량 데이터가 전송 또는 수신될 때, 필요한 기본 코드를 수행
	// message_id 251 : 클라이언트에게 대용량의 데이터를 전송할때 사용하는 예약번호
	// message_id 252 : 대용량의 데이터를 수신할 때 사용하는 예약번호 (아직 추가로 수신할 데이터가 있다)
	// message_id 253 : 대용량의 데이터를 수신할 때 사용하는 예약번호 (더이상 전송할 데이터가 없다)
	ServerSocket::ProcessRecvData(ah_socket, a_msg_id, ap_recv_data, a_body_size);
	
	
	UserData* p_user = ServerSocket::FindUserData(ah_socket); // 데이터를 보낸 클라이언트가 어떤 것인지 찾는다
	
	// 수신된 데이터가 채팅 데이터인 경우
	if (a_msg_id == NM_CHAT_DATA) 
	{
		TR("MyServer::ProcessRecvData - 수신된 데이터 (채팅 데이터)\n");

		CString str;
		str.Format(L"%s : %s", p_user->GetID(), (wchar_t*)ap_recv_data); // (wchar_t*)ap_recv_data 유니코드로 형변환
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
	
	// 로그인 데이터 (ap_recv_data: 사용자 'id/pw/name'이 넘어온다)
	else if (a_msg_id == NM_LOGIN_DATA) 
	{
		TR("MyServer::ProcessRecvData - 수신된 데이터 (로그인 데이터)\n");
		
	
		CString s1 = (wchar_t*)ap_recv_data;
		char* ch = new char[s1.GetLength() + 1];
		WideCharToMultiByte(CP_ACP, 0, s1, -1, ch, (s1.GetLength() + 1)*2, NULL, NULL); // 유니코드 -> 멀티바이트 변환
		

		// 문자열 자르고 보관하기
		char* sArr[3] = { NULL, };             // 넘어온 로그인 데이터를 구분자로 구분하여 각각 저장할 포인터 배열 (0:id, 1:pw, 2:name)
		int i = 0;                             // 문자열 포인터 배열의 인덱스

		char* ptr = strtok(ch, "/"); // 슬러시(/) 문자열을 기준으로 문자열을 자른다
		while (ptr != NULL)                    // 자른 문자열이 나오지 않을때 까지 반복
		{
			sArr[i] = ptr;                     // 자른 문자열의 메모리 주소를 문자열 포인터 배열에 저장
			i++;
			ptr = strtok(NULL, "/");           // 다음 문자열을 잘라서 포인터를 반환
		}


		// char -> wchar_t
		wchar_t* pStr;

		// ID 설정
		int strSize = MultiByteToWideChar(CP_ACP, 0, sArr[0], -1, NULL, NULL);       //멀티 바이트 크기 계산 길이 반환
		pStr = new WCHAR[strSize];                                                   //wchar_t 메모리 할당
		MultiByteToWideChar(CP_ACP, 0, sArr[0], strlen(sArr[0]) + 1, pStr, strSize); // 멀티바이트 -> 유니코드
		p_user->SetID(pStr);

		// PW 설정
		strSize = MultiByteToWideChar(CP_ACP, 0, sArr[1], -1, NULL, NULL);       
		pStr = new WCHAR[strSize];                                                 
		MultiByteToWideChar(CP_ACP, 0, sArr[1], strlen(sArr[1]) + 1, pStr, strSize); 
		p_user->SetPW(pStr);

		// Name 설정
		strSize = MultiByteToWideChar(CP_ACP, 0, sArr[2], -1, NULL, NULL);       
		pStr = new WCHAR[strSize];                                                  
		MultiByteToWideChar(CP_ACP, 0, sArr[2], strlen(sArr[2]) + 1, pStr, strSize);
		p_user->SetName(pStr);
		
		
		
		CString str;
		str.Format(L"%s 님이 접속하셨습니다 (%s)", p_user->GetID(), p_user->GetIP());
		mp_parent->AddEventString(str);


		str.Format(L"%s (%s)", p_user->GetID(), p_user->GetIP());
		mp_parent->AddUserString(str);
	}

	return 1; // 정상적으로 데이터를 받았으면 1반환 
	// 잘못된 사용자가 메시지를 보냈을때는 소켓을 끊고 0을 반환
}









// CServerDlg 대화 상자


CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent), m_server(this) // m_server(this): 객체를 생성할 때 대화상자의 주소를 넘겨준다
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	dlg_user_list = NULL;
}

CServerDlg::~CServerDlg()
{
	delete dlg_user_list;
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_event_list);
	DDX_Control(pDX, IDC_LIST2, m_user_list);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(25001, &CServerDlg::OnAcceptUser)
	ON_MESSAGE(25002, &CServerDlg::OnReadAndClose)
	ON_BN_CLICKED(IDC_START_BTN, &CServerDlg::OnBnClickedStartBtn)
	ON_BN_CLICKED(IDC_SEND_BTN, &CServerDlg::OnBnClickedSendBtn)
	ON_BN_CLICKED(IDC_DISCONNECT_BTN, &CServerDlg::OnBnClickedDisconnectBtn)
	ON_BN_CLICKED(IDC_SEND2_BTN, &CServerDlg::OnBnClickedSend2Btn)
	ON_BN_CLICKED(IDC_USERDATA_BTN, &CServerDlg::OnBnClickedUserdataBtn)
END_MESSAGE_MAP()


// m_event_list 메시지 추가 (채팅 메시지)
void CServerDlg::AddEventString(CString parm_string)
{
	int index = m_event_list.InsertString(-1, parm_string); // 리스트 목록 끝에(-1) 문자열(parm_string) 추가. 반환값(index): 추가되는 위치
	m_event_list.SetCurSel(index);                          // 추가한 곳(index) 커서 활성화
}

// m_user_list 메시지 추가 (사용자 목록)
void CServerDlg::AddUserString(CString parm_string)
{
	int index = m_user_list.InsertString(-1, parm_string);
	m_user_list.SetCurSel(index);
}

// m_user_list 목록 초기화
void CServerDlg::ResetUserString()
{
	m_user_list.ResetContent();
}


// CServerDlg 메시지 처리기

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.


	// '전송', '연결해제', '사용자 정보' 버튼 비활성화
	GetDlgItem(IDC_SEND_BTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_SEND2_BTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT_BTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_USERDATA_BTN)->EnableWindow(FALSE);



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



// Enter키 막기
BOOL CServerDlg::PreTranslateMessage(MSG* pMsg)
{
	//if (pMsg->wParam == VK_RETURN) return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}



// 새로운 클라이언트가 접속했을 때 발생하는 메시지를 처리한다
// FD_ACCEPT : 25001 메시지
afx_msg LRESULT CServerDlg::OnAcceptUser(WPARAM wParam, LPARAM lParam)
{
	TR("CServerDlg::OnAcceptUser - 새로운 클라이언트가 접속했을 때 발생하는 메시지를 처리한다 (25001)\n");
		
	m_server.ProcessToAccept(wParam, lParam); // ProcessToAccept: 클라이언트의 접속 처리(FD_ACCEPT 처리)

	dlg_user_list = m_server.GetUserList();   // 서버에 접속한 사용자 목록을 가져온다 (최초 호출시 ip값만 들어간다)

	return 0;
}


// 접속한 클라이언트가 데이터를 전송하거나 접속을 해제할 때 발생하는 메시지를 처리한다
// FD_READ, FD_CLOSE : 25002 메시지
afx_msg LRESULT CServerDlg::OnReadAndClose(WPARAM wParam, LPARAM lParam)
{
	TR("CServerDlg::OnReadAndClose - 접속한 클라이언트가 데이터를 전송하거나 접속을 해제할 때 발생하는 메시지를 처리한다 (25002)\n");
	
	m_server.ProcessClientEvent(wParam, lParam); // ProcessClientEvent : 클라이언트의 네트워크 이벤트 처리 (FD_READ, FD_CLOSE 처리 함수)

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

	int state = m_server.StartServer(ip, _ttoi(port), m_hWnd); // _ttoi : cstring -> int
	if (state == 1) // 정상적으로 서버 실행
	{
		AddEventString(L"서버 서비스를 시작합니다!");

		GetDlgItem(IDC_START_BTN)->EnableWindow(FALSE);
		GetDlgItem(IDC_SEND_BTN)->EnableWindow(TRUE);
		GetDlgItem(IDC_SEND2_BTN)->EnableWindow(TRUE);
		GetDlgItem(IDC_DISCONNECT_BTN)->EnableWindow(TRUE);
		GetDlgItem(IDC_USERDATA_BTN)->EnableWindow(TRUE);
	}
	else // StartServer 에서 에러 발생
	{
		MessageBox(L"서버를 실행할 수 없습니다\n(IP 주소를 확인해 주세요)", NULL, MB_OK);
	}
}


// '전송' 버튼 클릭
void CServerDlg::OnBnClickedSendBtn()
{
	if (m_server.GetUserCount() == 0)
	{
		AddEventString(L"서버에 접속한 사용자가 없습니다");
		SetDlgItemText(IDC_CHAT_EDIT, L"");
		return;
	}
	
	CString str, text;
	GetDlgItemText(IDC_CHAT_EDIT, text);
	str.Format(L"%s : %s", L"Server", text);

	for (int i = 0; i < m_server.GetUserCount(); i++)
	{
		// 현재 사용자가 접속 상태인지 확인한다
		if (dlg_user_list[i]->GetHandle() != INVALID_SOCKET)
		{
			m_server.SendFrameData(dlg_user_list[i]->GetHandle(), NM_CHAT_DATA, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2); // 데이터 전송
		}
	}

	AddEventString(str);
	SetDlgItemText(IDC_CHAT_EDIT, L"");
}




// '연결해제' 버튼 클릭 이벤트
void CServerDlg::OnBnClickedDisconnectBtn()
{
	if (m_server.GetUserCount() == 0)
	{
		MessageBox(L"서버에 접속한 사용자가 없습니다", NULL, MB_OK);
		return;
	}


	// m_user_list 에서 선택한 셀의 텍스트를 가져온다
	CString str;
	int n = m_user_list.GetCurSel();
	if (n == -1) // 선택된 셀이 없으면
	{
		MessageBox(L"연결을 해제할 사용자를 선택해주세요", NULL, MB_OK);
		return;
	}

	m_user_list.GetText(n, str); // 선택된 셀(n)의 문자열을 가져옴


	// strtok 함수의 매개변수로 사용하기 위해 CString 문자열을 char*로 변환한다
	char* ch = new char[str.GetLength() + 1];
	WideCharToMultiByte(CP_ACP, 0, str, -1, ch, (str.GetLength() + 1) * 2, NULL, NULL); // 유니코드 -> 멀티바이트 변환


	// 문자열 파싱 (strtok)
	char del[] = " ";     // 구분자
	ch = strtok(ch, del); // 문자열 파싱 (ch문자열을 del구분자로 나눈다)

	str = (CString)ch;
	
	delete[] ch;


	// 추출한 ID를 비교하여 해당 소켓을 제거
	for (int i = 0; i < MAX_CLIENT_COUNT; i++)
	{
		if (dlg_user_list[i]->GetHandle() != INVALID_SOCKET)
		{
			if (dlg_user_list[i]->GetID() == str)
			{
				m_server.DisconnectSocket(dlg_user_list[i]->GetHandle(), 0);
			}
		}
	}

}


// '전송(선택)' 버튼 클릭 이벤트
// 특정 사용자에게만 메시지 전송
void CServerDlg::OnBnClickedSend2Btn()
{
	// m_user_list 에서 선택한 셀의 텍스트를 가져온다
	CString id;
	int n = m_user_list.GetCurSel();
	if (n == -1) // 선택된 셀이 없으면
	{
		MessageBox(L"메시지를 보낼 사용자를 선택해주세요", NULL, MB_OK);
		return;
	}
	
	m_user_list.GetText(n, id);


	// strtok 함수의 매개변수로 사용하기 위해 CString 문자열을 char*로 변환한다
	char* ch = new char[id.GetLength() + 1];
	WideCharToMultiByte(CP_ACP, 0, id, -1, ch, (id.GetLength() + 1) * 2, NULL, NULL); // 유니코드 -> 멀티바이트 변환


	// 문자열 파싱 (strtok)
	char del[] = " ";     // 구분자
	ch = strtok(ch, del); // 문자열 파싱 (ch문자열을 del구분자로 나눈다)

	id = (CString)ch;

	delete[] ch;


	CString chat;
	GetDlgItemText(IDC_CHAT2_EDIT, chat);

	CString str;
	str.Format(L"Server : %s", chat);

	// 추출한 ID를 비교하여 해당 소켓을 제거
	for (int i = 0; i < MAX_CLIENT_COUNT; i++)
	{
		if (dlg_user_list[i]->GetHandle() != INVALID_SOCKET)
		{
			if (dlg_user_list[i]->GetID() == id)
			{
				m_server.SendFrameData(dlg_user_list[i]->GetHandle(), NM_CHAT_DATA, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);
			}
		}
	}

	SetDlgItemText(IDC_CHAT2_EDIT, L"");
}


// '사용자 정보' 버튼 이벤트
void CServerDlg::OnBnClickedUserdataBtn()
{
	int n = m_user_list.GetCurSel();
	if (n == -1) // 선택된 셀이 없으면
	{
		MessageBox(L"메시지를 보낼 사용자를 선택해주세요", NULL, MB_OK);
		return;
	}
	
	CString id;
	m_user_list.GetText(n, id);


	// strtok 함수의 매개변수로 사용하기 위해 CString 문자열을 char*로 변환한다
	char* ch = new char[id.GetLength() + 1];
	WideCharToMultiByte(CP_ACP, 0, id, -1, ch, (id.GetLength() + 1) * 2, NULL, NULL); // 유니코드 -> 멀티바이트 변환


	// 문자열 파싱 (strtok)
	char del[] = " ";     // 구분자
	ch = strtok(ch, del); // 문자열 파싱 (ch문자열을 del구분자로 나눈다)
	id = (CString)ch;
	delete[] ch;

	for (int i = 0; i < MAX_CLIENT_COUNT; i++)
	{
		if (dlg_user_list[i]->GetHandle() != INVALID_SOCKET)
		{
			if (dlg_user_list[i]->GetID() == id)
			{
				CString str;
				str.Format(L"IP: %s\nId: %s\nPw: %s\nName: %s", dlg_user_list[i]->GetIP(), dlg_user_list[i]->GetID(), dlg_user_list[i]->GetPW(), dlg_user_list[i]->GetName());
				MessageBox(str, NULL, MB_OK);
			}
		}
	}


}
