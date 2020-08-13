
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



// 수신된 데이터를 처리하는 함수
int MyClient::ProcessRecvData(SOCKET ah_socket, unsigned char a_msg_id, char* ap_recv_data, BS a_body_size)
{
	// 대용량 데이터가 전송 또는 수신될 때, 필요한 기본 코드를 수행
	ClientSocket::ProcessRecvData(ah_socket, a_msg_id, ap_recv_data, a_body_size);
	
	
	if (a_msg_id == NM_CHAT_DATA) // 수신된 데이터가 채팅 데이터인 경우
	{
		// 수신된 데이터에 대한 채팅 정보가 유니코드 형태의 문자열로 저장되어 있기 때문에 
		// 유니코드 문자열 형태로 캐스팅하여 리스트 박스에 추가
		mp_parent->AddEventString((wchar_t*)ap_recv_data);
	}

	return 1;
}



// CClientDlg 대화 상자

CClientDlg::CClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENT_DIALOG, pParent), m_client(this) // m_client(this): 객체를 생성할 때 대화상자의 주소를 넘겨준다
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
	ON_BN_CLICKED(IDC_SEND_BTN, &CClientDlg::OnBnClickedSendBtn)
	ON_MESSAGE(26001, &CClientDlg::OnConnected)
	ON_MESSAGE(26002, &CClientDlg::OnReadAndClose)
	ON_BN_CLICKED(IDC_CONNECT_BTN, &CClientDlg::OnBnClickedConnectBtn)
END_MESSAGE_MAP()


// CClientDlg 메시지 처리기

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	
	// 서버에 접속하기 (IP 주소가 '192.168.77.100'을 사용하고 포트 번호가 27100인 서버에 접속을 시도한다)
	//m_client.ConnectToServer(L"192.168.77.100", 27100, m_hWnd);
	

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




// 리스트 박스 메시지 추가
void CClientDlg::AddEventString(CString parm_string)
{
	int index = m_event_list.InsertString(-1, parm_string); // 리스트 목록 끝에(-1) 문자열(parm_string) 추가. 반환값(index): 추가되는 위치
	m_event_list.SetCurSel(index);                          // 추가한 곳(index) 커서 활성화
}




// '전송' 버튼 클릭 이벤트
void CClientDlg::OnBnClickedSendBtn()
{
	CString str;
	GetDlgItemText(IDC_CHAT_EDIT, str); // IDC_CHAT_EDIT 에디트 박스에 입력된 문자열을 str에 가져온다
	SetDlgItemText(IDC_CHAT_EDIT, L"");

	// 서버와 접속 상태인지 확인한다
	if (m_client.IsConnected() == 1) // 서버에 접속중
	{
		m_client.SendFrameData(NM_CHAT_DATA, (char*)(const wchar_t*)str, (str.GetLength() + 1) * 2); // 서버에 채팅 데이터 전송
		// 문자열을 전송할 때는 NULL 문자를 포함해서 전송하기 때문에 문자열 길이에 1을 더하고 
		// 유니코드에서 문자 1개는 2byte를 차지하기 때문에 문자열 길이에 2를 곱한다
	}

}




// 서버 접속에 대한 결과를 알려주는 메시지를 처리한다 
// FD_CONNECT : 26001 메시지
afx_msg LRESULT CClientDlg::OnConnected(WPARAM wParam, LPARAM lParam)
{
	// 서버 접속에 대한 결과
	if (m_client.ResultOfConnection(lParam) == 1) // 접속 성공
	{
		AddEventString(L"서버에 접속했습니다!");
	}
	else // 접속 실패
	{
		AddEventString(L"서버에 접속할수 없습니다!");
	}
	
	return 0;
}


// 접속한 서버에서 데이터를 전송하거나 접속을 해제할 때 발생하는 메시지를 처리한다
// FD_READ, FD_CLOSE : 26002 메시지
afx_msg LRESULT CClientDlg::OnReadAndClose(WPARAM wParam, LPARAM lParam)
{
	if (m_client.ProcessServerEvent(wParam, lParam) == 0) // ProcessServerEvent: FD_READ는 1, FD_CLOSE는 0 반환
	{
		AddEventString(L"서버와 연결이 해제되었습니다.");
	}

	return 0;
}


// '서버 접속' 버튼 클릭
void CClientDlg::OnBnClickedConnectBtn()
{
	// 서버와 접속 상태인지 확인하여 서버와 접속중이라면 처리하지 않는다
	if (!m_client.IsConnected()) // 서버에 접속중이 아니라면
	{
		CString str;
		GetDlgItemText(IDC_IP_EDIT, str); // 사용자가 입력한 주소를 str에 가져온다
		if (!str.IsEmpty())
		{
			AddEventString(L"서버에 접속을 시도합니다. : " + str);			
			m_client.ConnectToServer(str, 27100, m_hWnd); // 사용자가 접속한 IP 주소에 포트 번호가 27100인 서버로 접속을 시도한다
		}
	}
	else
	{
		AddEventString(L"이미 서버에 접속중 입니다.");
	}

}