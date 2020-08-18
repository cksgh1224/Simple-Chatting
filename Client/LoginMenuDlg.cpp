// LoginMenuDlg.cpp: 구현 파일
//

#include "pch.h"
#include "Client.h"
#include "LoginMenuDlg.h"
#include "afxdialogex.h"

#include "ClientDlg.h"


// LoginMenuDlg 대화 상자

IMPLEMENT_DYNAMIC(LoginMenuDlg, CDialogEx)

LoginMenuDlg::LoginMenuDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGINMENU_DIALOG, pParent)
{

}

LoginMenuDlg::~LoginMenuDlg()
{
}

void LoginMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LoginMenuDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &LoginMenuDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &LoginMenuDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_DELETE_BTN, &LoginMenuDlg::OnBnClickedDeleteBtn)
END_MESSAGE_MAP()


// LoginMenuDlg 메시지 처리기


// 엔터 종료 막기
BOOL LoginMenuDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


// '채팅' 버튼 클릭 이벤트
void LoginMenuDlg::OnBnClickedButton1()
{
	login_data = select_data;

	::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL); // 대화상자 종료
	CClientDlg dlg(login_data); // 채팅창 띄우기
	dlg.DoModal();
}


// '회원 정보 수정' 버튼 클릭 이벤트
void LoginMenuDlg::OnBnClickedButton2()
{
	// 미구현
	MessageBox(L"회원 정보 수정", L"미구현", MB_OK);
}


// '회원 탈퇴' 버튼 클릭 이벤트
void LoginMenuDlg::OnBnClickedDeleteBtn()
{
	// 미구현
	MessageBox(L"회원 탈퇴", L"미구현", MB_OK);
}
