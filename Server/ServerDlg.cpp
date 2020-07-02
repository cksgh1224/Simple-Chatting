
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
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


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


// WindowProc: 윈도우에 메시지가 들어왔을 때 호출되는 함수
LRESULT CServerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// wParam: 메시지가 발생하게된 소켓의 핸들(mh_listen_socket)
	// lParam: 소켓에 에러가 있는지(WSAGETSELECTERROR) or 어떤 이벤트 때문에 발생했는지(WSAGETSELECTEVENT)

	if (27001 == message) { // // FD_ACCEPT (소켓에 클라이언트가 접속하려고 할 때 발생)
		if (MAX_CLIENT_COUNT > m_client_count) {
			SOCKET h_socket = (SOCKET)wParam; // h_socket -> mh_listen_socket 핸들값과 동일

			// accept() : 클라이언트 연결 수립
			// listen_socket의 복제를 만들어서 반환 -> 클라이언트와 실제 통신  (listen_socket은 클라이언트의 접속만 받음)
			struct sockaddr_in client_addr;
			int sockaddr_in_size = sizeof(client_addr);
			mh_client_list[m_client_count] = accept(h_socket, (LPSOCKADDR)&client_addr, &sockaddr_in_size); // mh_client_list[] : 실제 통신할 소켓

			WSAAsyncSelect(mh_client_list[m_client_count], m_hWnd, 27002, FD_READ | FD_CLOSE); // recv, close -> 비동기 처리

			m_client_count++;

			// 접속한 클라이언트의 ip
			CString ip_address; // CString은 MFC에서 문자열을 처리를 아주 쉽게 처리할 수 있도록 제공해주는 클래스
			ip_address = inet_ntoa(client_addr.sin_addr);
			MessageBox(ip_address, L"새로운 클라이언트가 접속했습니다", MB_OK); // 유니코드 문자집합 -> 문자열 앞에 L 을 붙여준다
		}
		else {

		}
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}
