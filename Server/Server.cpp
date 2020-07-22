
// Server.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "Server.h"
#include "ServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerApp

BEGIN_MESSAGE_MAP(CServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CServerApp 생성

CServerApp::CServerApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CServerApp 개체입니다.

CServerApp theApp;


// CServerApp 초기화

BOOL CServerApp::InitInstance()
{
	CWinApp::InitInstance();

	CServerDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal(); // 다이얼로그 실행

	return FALSE;
}

