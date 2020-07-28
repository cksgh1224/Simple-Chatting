
// Client.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "Client.h"
#include "ClientDlg.h"
#include "LoginDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientApp

BEGIN_MESSAGE_MAP(CClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CClientApp 생성

CClientApp::CClientApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CClientApp 개체입니다.

CClientApp theApp;


// CClientApp 초기화

BOOL CClientApp::InitInstance()
{
	CWinApp::InitInstance();

	LoginDialog dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}

