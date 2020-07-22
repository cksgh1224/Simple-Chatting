﻿
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
void MyServer::AddWorkForAccept(TW_UserData* ap_user)
{
	CString str;
	str.Format(L"%s에서 새로운 사용자가 접속했습니다!", ap_user->GetIP());
	mp_parent->AddEventString(str);
}


// 최대 사용자수 초과시에 추가적으로 해야할 작업 처리
void MyServer::ShowLimitError(const wchar_t* ap_ip_address)
{
	
}


// 클라이언트 접속 해제시에 추가적으로 해야할 작업 처리
// 매개변수로 넘어오는 UserData* ap_user는 접속을 해제하는 사용자(클라이언트)의 정보를 담고있다
// a_error_code : 0이면 정상종료, -1이면 키값이 유효하지 않아서 종료, -2이면 바디정보 수신중에 오류 발생
void MyServer::AddWorkForCloseUser(TW_UserData* ap_user, int a_error_code)
{
	CString str;
	str.Format(L"%s에서 새로운 사용자가 접속을 해제했습니다!", ap_user->GetIP());
	mp_parent->AddEventString(str);
}


// 수신된 데이터를 처리하는 함수
// ah_socket: 데이터를 전송한 클라이언트 소켓의 핸들, a_msg_id: 클라이언트가 보낸 데이터가 어떤 종류의 데이터인지
// ap_recv_data: 클라이언트가 보낸 데엍, a_body_size: 클라이언트가 보낸 데이터의 body size (헤더를 뺀 바디 크기)
int MyServer::ProcessRecvData(SOCKET ah_socket, unsigned char a_msg_id, char* ap_recv_data, BS a_body_size)
{
	TW_UserData* p_user = TW_ServerSocket::FindUserData(ah_socket); // 데이터를 보낸 클라이언트가 어떤 것인지 찾는다
	
	if (a_msg_id == NM_CHAT_DATA) // 클라이언트가 보낸 데이터가 채팅 데이터 이면
	{
		CString str;
		str.Format(L"%s : %s", p_user->GetIP(), (wchar_t*)ap_recv_data); // (wchar_t*)ap_recv_data 유니코드로 형변환
		mp_parent->AddEventString(str);

		// 서버에 연결된 모든 클라이언트들에게 수신된 데이터를 전송
		for (int i = 0; i < m_max_user_count; i++)
		{
			// 현재 사용자가 접속 상태인지 확인한다
			if (mp_user_list[i]->GetHandle() != -1)
			{
				TW_Socket::SendFrameData(mp_user_list[i]->GetHandle(), NM_CHAT_DATA, (const char*)(const wchar_t*)str, (str.GetLength() + 1) * 2);
				// (str.GetLength() + 1) * 2 -> \0 때문에 +1을 해주고 유니코드는 문자 하나가 2byte이므로 x2를 해준다
			}
		}

	}

	return 1; // 정상적으로 데이터를 받았으면 1반환 
	// 잘못된 사용자가 메시지를 보냈을때는 소켓을 끊고 0을 반환
}





// CServerDlg 대화 상자



CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent), m_server(this)
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
END_MESSAGE_MAP()


// 리스트 박스 메시지 추가
void CServerDlg::AddEventString(CString parm_string)
{
	int index = m_event_list.InsertString(-1, parm_string); // 리스트 목록 끝에(-1) 문자열(parm_string) 추가. 반환값(index): 추가되는 위치
	m_event_list.SetCurSel(index); // 추가한 곳(index) 커서 활성화
}

void CServerDlg::AddEventString(const wchar_t* ap_string)
{
	int index = m_event_list.InsertString(-1, ap_string);
	m_event_list.SetCurSel(index);
}


// CServerDlg 메시지 처리기

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.


	// 서버 서비스 시작 (socket - bind - listen)
	m_server.StartServer(L"192.168.77.100", 27100, m_hWnd);
	AddEventString(L"서버 서비스를 시작합니다!");

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



afx_msg LRESULT CServerDlg::OnAcceptUser(WPARAM wParam, LPARAM lParam)
{
	// 새로운 클라이언트가 접속했을 때 발생하는 메시지를 처리한다
	m_server.ProcessToAccept(wParam, lParam);

	return 0;
}


afx_msg LRESULT CServerDlg::OnReadAndClose(WPARAM wParam, LPARAM lParam)
{
	// 접속한 클라이언트가 데이터를 전송하거나 접속을 해제할 때 발생하는 메시지를 처리한다
	m_server.ProcessClientEvent(wParam, lParam);
	
	return 0;
}
