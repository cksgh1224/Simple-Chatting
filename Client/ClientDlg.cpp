
// ClientDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientDlg 대화 상자



CClientDlg::CClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_event_list);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// 리스트 박스 메시지 추가
void CClientDlg::AddEventString(CString parm_string)
{
	int index = m_event_list.InsertString(-1, parm_string); // 리스트 목록 끝에(-1) 문자열(parm_string) 추가. 반환값(index): 추가되는 위치
	m_event_list.SetCurSel(index); // 추가한 곳(index) 커서 활성화
}

// CClientDlg 메시지 처리기

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// socket() : 소켓 생성
	mh_socket = socket(AF_INET, SOCK_STREAM, 0); 

	// 소켓의 네트워크 설정 (주소 체계, IP, 포트번호) (어떤 서버에 접속할 것인가) 
	sockaddr_in srv_addr;
	srv_addr.sin_family = AF_INET; // 접속할 서버의 주소 체계
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 소켓이 연결할 네트워크 카드 설정(IP)
	srv_addr.sin_port = htons(20001); // 접속할 서버의 포트번호

	// connect() : 서버에 접속 (연결 요청)
	// 서버에 문제가 있어 접속이 지연되는 경우 connect() 함수에서 30초정도 빠져나가지 못하고 묶여있는다 -> 비동기 방식으로 처리
	// FD_CONNECT: 접속을 시도했을 때 발생하는 것이 아닌, 접속 시도 행위에 대한 결과이다 (접속에 성공했을 수도 있고, 실패했을 수도 있는거임)
	// 위와같이 비동기를 걸어주면, connect() 함수는 서버에 상태와 상관없이 들어가자마자 바로 나온다. 접속 시도만 하고 빠져나온다.
	// 서버에 접속 시도를 해서, 그 시도에 대한 결과가 주어지면 (FD_CONNECT) -> 27001 메시지 발생
	// 비동기 방식을 이용 -> 서버에 문제가 있어도 connect() 함수에서 기다리지 않고 바로 빠져나오고, 서버 접속에 대한 결과가 나오면(FD_CONNECT) 그때 27001 메시지를 발생시킨다

	WSAAsyncSelect(mh_socket, m_hWnd, 27001, FD_CONNECT); // 이 소켓에, 서버 접속 시도에 대한 결과가 정해지면, 윈도우에게, 27001 메시지를 발생시켜라
	connect(mh_socket, (sockaddr*)&srv_addr, sizeof(srv_addr));

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CClientDlg::OnPaint()
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
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// WindowProc: 윈도우에 메시지가 들어왔을 때 호출되는 함수
LRESULT CClientDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// wParam: 메시지가 발생하게된 소켓의 핸들(mh_listen_socket)
	// lParam: 소켓에 에러가 있는지(WSAGETSELECTERROR) or 어떤 이벤트 때문에 발생했는지(WSAGETSELECTEVENT)

	if (message == 27001) // FD_CONNECT (접속 시도에 대한 결과 : 성공 or 실패)
	{
		if (WSAGETSELECTERROR(lParam) == 0) // 에러가 없다 (서버 접속 성공)
		{
			// 서버에 접속 성공 -> 비동기를 새로 건다 (FD_READ: 서버에서 데이터를 보냄, FD_CLOSE: 서버가 연결을 끊음)
			WSAAsyncSelect(mh_socket, m_hWnd, 27002, FD_READ | FD_CLOSE); 
			AddEventString(L"서버에 접속했습니다!");
		}
		else // 서버 접속 실패
		{
			closesocket(mh_socket); // 소켓을 닫는다 closesocket을 했다고 해서 mh_socket이 초기화되는 것은 아님
			mh_socket = INVALID_SOCKET; // 소켓을 확실히 닫았다 (mh_socket 초기화) (mh_socket에 INVALID_SOCKET이 들어있으면 소켓이 끊어진 상태, 그렇지 않으면 접속되어 있는 상태)
			AddEventString(L"서버에 접속할 수 없습니다!");
		}
		
	}


	return CDialogEx::WindowProc(message, wParam, lParam);
}
