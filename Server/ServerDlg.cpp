
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


// CServerDlg 대화 상자



CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_client_count = 0;
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_event_list);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// 리스트 박스 메시지 추가
void CServerDlg::AddEventString(CString parm_string)
{
	int index = m_event_list.InsertString(-1, parm_string); // 리스트 목록 끝에(-1) 문자열(parm_string) 추가. 반환값(index): 추가되는 위치
	m_event_list.SetCurSel(index); // 추가한 곳(index) 커서 활성화
}


// CServerDlg 메시지 처리기

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// socket() : 소켓 생성 (AF_INET: 주소체계, SOCK_STREAM: TCP)
	mh_listen_socket = socket(AF_INET, SOCK_STREAM, 0); // 클라이언트의 접속을 받아주는 소켓

	// 소켓의 네트워크 설정 (주소 체계, IP, 포트번호)
	struct sockaddr_in srv_addr;
	srv_addr.sin_family = AF_INET; // 주소 체계
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 소켓이 연결할 네트워크 카드 설정(IP)
	srv_addr.sin_port = htons(20001); // 포트번호 설정 (이 프로그램에서 사용할 고유의 포트번호로 20001 설정) 

	// bind() : 서버 소켓 바인딩 (네트워크 카드에 소켓 연결)
	// mh_listen_socket을 "127.0.0.1" IP를 가진 네트워크 카드에 연결, (주소체계는 AF_INET, 포트번호는 20001)
	bind(mh_listen_socket, (LPSOCKADDR)&srv_addr, sizeof(srv_addr)); // LPSOCKADDR == SOCKADDR *
	
	// listen() : 클라이언트의 연결 요청 대기 
	listen(mh_listen_socket, 1); // 1: 대기자수 (여러 클라이언트가 동시에 접속하더라도 한번에 1개만 처리)
	AddEventString(L"클라이언트의 접속을 허락합니다...");

	// accept : 클라이언트 연결 수립 (실제 클라이언트의 접속) -> 클라이언트가 접속할 때 까지 이 함수에서 못 빠져나온다 -> 비동기 처리를 해줘야함
	// WSAAsyncSelect : 특정 소켓에 비동기 설정
	WSAAsyncSelect(mh_listen_socket, m_hWnd, 27001, FD_ACCEPT); // 이 소켓(mh_listen_socket)에 클라이언트가 접속하려고 한다면(FD_ACCEPT) 이 윈도우(m_hWnd)에게 27001 메시지를 준다 => WindowProc에서 처리 (m_hWnd : 대화상자의 윈도우 핸들)
	
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


// 클라이언트 접속 시도 처리 (FD_ACCEPT)
void CServerDlg::AcceptProcess(SOCKET parm_h_socket)
{
	// parm_h_socket : mh_listen_socket 핸들값과 동일 (메시지를 발생시킨 소켓)

	if (MAX_CLIENT_COUNT > m_client_count)
	{	
		// accept() : 클라이언트 연결 수립
		// listen_socket의 복제를 만들어서 반환 -> 클라이언트와 실제 통신  (listen_socket은 클라이언트의 접속만 받음)
		struct sockaddr_in client_addr;
		int sockaddr_in_size = sizeof(client_addr);
		mh_client_list[m_client_count] = accept(parm_h_socket, (LPSOCKADDR)&client_addr, &sockaddr_in_size); // mh_client_list[] : 실제 통신할 소켓

		WSAAsyncSelect(mh_client_list[m_client_count], m_hWnd, 27002, FD_READ | FD_CLOSE); // recv, close -> 비동기 처리

		// 접속한 클라이언트의 ip
		CString ip_address; // CString은 MFC에서 문자열을 처리를 아주 쉽게 처리할 수 있도록 제공해주는 클래스
		ip_address = inet_ntoa(client_addr.sin_addr);
		wcscpy(m_client_ip[m_client_count], ip_address); // 접속한 사용자의 ip를 배열에 보관 (wcscpy: strcpy의 유니코드 버전)
		m_client_count++;

		// MessageBox(ip_address, L"새로운 클라이언트가 접속했습니다", MB_OK); // 유니코드 문자집합 -> 문자열 앞에 L 을 붙여준다
		AddEventString(L"새로운 클라이언트가 접속했습니다 : " + ip_address);
	}
	else // 접속 인원 초과
	{

	}
}

// 클라이언트의 접속 해제 (FD_CLOSE)
void CServerDlg::ClientCloseProcess(SOCKET parm_h_socket, char parm_force_flag)
{
	// closesocket() 의 예외사항 -> 해당 소켓으로 데이터를 보내거나 받고 있으면 처리가 끝날때까지 기다린다 -> char param_force_flag 이용

	if (parm_force_flag == 1) // parm_force_flag 값이 1이면 데이터가 송수신되는 것과 상관없이 소켓을 바로 닫겠다
	{
		LINGER temp_linger = { TRUE, 0 }; // 데이터가 송수신되는 것과 상관없이 소켓을 바로 닫겠다 (TRUE, 0)
		setsockopt(parm_h_socket, SOL_SOCKET, SO_LINGER, (char*)&temp_linger, sizeof(temp_linger)); // param_h_socket소켓의 LINGER 옵션을 내가 설정한 LINGER 옵션으로 바꾼다		
	}
	
	closesocket(parm_h_socket);

	for (int i = 0; i < m_client_count; i++)
	{
		if (parm_h_socket == mh_client_list[i]) 
		{
			m_client_count--;
			if (i != m_client_count) // 배열의 마지막 소켓이 아니면
			{
				mh_client_list[i] = mh_client_list[m_client_count]; // mh_client_list 배열에서 해제할 소켓 위치에 배열의 마지막 소켓을 넣는다
				wcscpy(m_client_ip[i], m_client_ip[m_client_count]);
			}		
		}
	}
}

// recv (body 데이터 읽기)
void CServerDlg::ReceiveData(SOCKET parm_h_socket, char* p_body_data, unsigned short body_size)
{
	int current_size, total_size = 0, retry_count = 0;
	// current_size: recv 함수의 반환값을 저장할 변수 (recv함수는 읽은 값 만큼 반환)
	// total_size: recv함수의 반환값을 모두 더해서 전체 데이터를 다 받았는지 확인할 변수 (total_size가 body_size와 같으면 다 읽은거)					 
	// retry_count: 재시도 값 (recv에서 에러 발생시)

	// recv() 함수: 1000byte 데이터를 읽어라 -> 한번에 1000byte 다 읽지 못할 수도 있다 -> 400byte 읽고 반환, 600byte 읽고 반환) -> 반복문 이용
	while (total_size < body_size) // 데이터를 다 읽지 못했으면
	{
		current_size = recv(parm_h_socket, p_body_data + total_size, body_size - total_size, 0); // h_socket으로부터 (p_body_data + total_size) 메모리 위치에서 (body_size - total_size)만큼 데이터를 읽어서 저장해라

		if (current_size == SOCKET_ERROR) // 수신중에 에러가 발생하면 SOCKET_ERROR라는 값을 반환한다
		{
			retry_count++;
			Sleep(50); // 에러 발생시 0.05초 쉬었다가 다시 시도
			if (retry_count > 5) // 에러가 한번 발생했다고 바로 끊어버리지 않고, 6번 재시도 하겠다
				break;
		}
		else
		{
			retry_count = 0;
			total_size += current_size;
		}
	}
}





// WindowProc: 윈도우에 메시지가 들어왔을 때 호출되는 함수
LRESULT CServerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// wParam: 메시지가 발생하게된 소켓의 핸들(mh_listen_socket)
	// lParam: 소켓에 에러가 있는지(WSAGETSELECTERROR) or 어떤 이벤트 때문에 발생했는지(WSAGETSELECTEVENT)

	if (message == 27001) { // // FD_ACCEPT (소켓에 클라이언트가 접속하려고 할 때 발생)
		AcceptProcess((SOCKET)wParam);
	}
	else if (message == 27002) // FD_READ, FD_CLOSE
	{ 
		SOCKET h_socket = (SOCKET)wParam; // mh_listen_socket

		if (WSAGETSELECTEVENT(lParam) == FD_READ) // FD_READ 발생 (클라이언트가 데이터를 보냈다)
		{
			// FD_READ 발생하는 경우
			// 1. 수신 버퍼가 비어있을 때 새로운 데이터가 들어오면
			// 2. WSAAsyncSelect로 FD_READ 비동기를 걸 때 수신 버퍼에 데이터가 있으면
			// 3. recv() 함수를 사용해서 수신 버퍼에세 데이터를 가져온 이후에도 수신 버퍼에 데이터가 있으면 

			// 수신, 송신 측에서 데이터를 주고 받을 때 -> 헤더를 만들어서(주고받는 데이터에 대한 정보) 송수신
			// 4byte 크기의 헤더 -> 1byte: 이 프로토콜이 정상적인 프로토콜인지, 2byte: 바디(실제 데이터)크기가 얼마인지, 1byte: 이 데이터가 어떤 종류의 데이터인지
			// 헤더 -> 4byte를 한번의 읽지 않고 끊어서 읽는다. 1byte, 2byte, 1byte
			// 끊어 읽기를 하면 (FD_READ가 발생하는 3번 이유 떄문에) FD_READ가 계속 발생한다
			// => 헤더, 바디를 끊어 읽기할 때는 FD_READ가 발생하지 않게 해주자 -> 비동기 다시 걸기

			WSAAsyncSelect(h_socket, m_hWnd, 27002, FD_CLOSE); // FD_CLOSE에 의해서만 발생하도록 설정 (원래 27002 메시지는 FD_READ와 FD_CLOSE에 의해서 발생)
			// recv함수를 몇번을 쓰던 FD_READ는 발생하지 않는다
			
			char key, network_message_id; // key: 이 프로토콜이 정상적인 프로토콜인지 체크 (1byte), network_message_id: 이 데이터가 어떤 종류의 데이터인지 
			recv(h_socket, &key, sizeof(key), 0); // h_socket으로부터 1바이트의 데이터를 key에다가 가져옴
			if (key == 27) // key값이 27이면 내가 원하는 클라이언트에서 전송한 데이터 (27 내가 설정한 임의의 값)
			{
				unsigned short body_size; // 바디(실제 데이터)크기를 담을 변수 (2byte)
				recv(h_socket, (char*)&body_size, sizeof(body_size), 0); // body_size를 가져옴
				recv(h_socket, &network_message_id, 1, 0); // network_message_id를 가져옴
				// ↑↑↑ 헤더를 다 읽어들임 

				// body 읽어들이기 
				if (body_size > 0) // body(데이터)가 없을 수도 있기 때문에 체크
				{
					char* p_body_data = new char[body_size]; // body_size만큼 메모리 동적 할당

					ReceiveData(h_socket, p_body_data, body_size); // body 데이터 읽기
					
					if (network_message_id == 1)
					{
						// 실제로 클라이언트가 보내준 데이터(p_body_data)를 처리 
					}

					delete[] p_body_data;
				}

				WSAAsyncSelect(h_socket, m_hWnd, 27002, FD_READ | FD_CLOSE); // 데이터를 다 읽은 후 비동기 재설정
			}
			else // 내가 원하는 클라이언트에서 전송한 데이터가 아님 -> 소켓을 즉각 파괴시킨다
			{
				ClientCloseProcess(h_socket, 1);
			}

		}
		else // FD_CLOSE (클라이언트의 접속 해제)
		{
			ClientCloseProcess(h_socket, 0);
		}

	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}
