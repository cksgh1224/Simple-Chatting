// LoginDialog.cpp: 구현 파일
//

#include "pch.h"
#include "LoginDlg.h"
#include "Client.h"
#include "afxdialogex.h"


#include "LoginMenuDlg.h"        // 로그인 메뉴 대화상자
#include "ClientDlg.h"           // 채팅 대화상자
#include "CreateAccountDlg.h"    // 회원가입 대화상자
#include "FindIdDlg.h"           // 아이디 찾기 대화상자
#include "FindPwDlg.h"           // 비밀번호  찾기 대화상자



// LoginDialog 대화 상자

IMPLEMENT_DYNAMIC(LoginDialog, CDialogEx)

LoginDialog::LoginDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_DIALOG, pParent)
{
	
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LoginDialog, CDialogEx)
	ON_BN_CLICKED(IDC_LOGIN_BTN, &LoginDialog::OnBnClickedLoginBtn)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SELECT_BTN, &LoginDialog::OnBnClickedSelectBtn)
	ON_BN_CLICKED(IDC_JOIN_BTN, &LoginDialog::OnBnClickedJoinBtn)
	ON_BN_CLICKED(IDC_FINDID_BTN, &LoginDialog::OnBnClickedFindidBtn)
	ON_BN_CLICKED(IDC_FINDPW_BTN, &LoginDialog::OnBnClickedFindpwBtn)
END_MESSAGE_MAP()


// LoginDialog 메시지 처리기


// 다이얼로그 엔터 종료 막기
BOOL LoginDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN) return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}



BOOL LoginDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	// mysql 서버 연결
	if (my_odbc.Connect(L"socket_user", L"root", L"0000", this))
	{
		//MessageBox(L"서버 접송 성공", L"MySql", MB_OK);
		OutputDebugString(L"MySql 서버 접속\n");
	}
	else
	{
		MessageBox(L"서버 접송 실패", L"MySql", MB_OK);
		OutputDebugString(L"MySql 서버 접속 실패\n");
	}
		
	return TRUE;
}



// WM_DESTROY 메시지 처리
void LoginDialog::OnDestroy()
{
	CDialogEx::OnDestroy();

	my_odbc.Disconnect(); // 서버 연결 해제
}



// '전체 사용자' 버튼 클릭 이벤트
void LoginDialog::OnBnClickedSelectBtn()
{
	CString query = L" select mid, mpw, mname from user ";
	my_odbc.ExecQuery(query, sizeof(UserAccount), SetRecordInfo, ResultRecord, 0);
}



// '로그인' 버튼 클릭 이벤트
void LoginDialog::OnBnClickedLoginBtn()
{
	CString id, pw, query;
	GetDlgItemText(IDC_ID_EDIT, id);
	GetDlgItemText(IDC_PW_EDIT, pw);

	if (id.IsEmpty() || pw.IsEmpty())
	{	
		MessageBox(L"아이디, 비밀번호를 입력해 주세요", NULL, MB_OK);
		return;
	}
	
	query.Format(L" select * from user where mid='%s' and mpw='%s' ", id, pw);

	if (my_odbc.ExecQuery(query, sizeof(UserAccount), SetRecordInfo, ResultRecord, 1)) // query 실행
	{
		::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL); // 대화상자 종료
		LoginMenuDlg dlg; // 채팅창 띄우기
		dlg.DoModal();
	}
	else
	{
		MessageBox(L"입력하신 정보가 일치하지 않습니다", NULL, MB_OK);
	}

} 



// '회원가입' 버튼 클릭 이벤트
void LoginDialog::OnBnClickedJoinBtn()
{
	CreateAccountDlg dlg(my_odbc);
	dlg.DoModal();
}



// '아이디 찾기' 버튼 클릭 이벤트
void LoginDialog::OnBnClickedFindidBtn()
{
	FindIdDlg dlg(my_odbc);
	dlg.DoModal();
}



// '비밀번호 찾기' 버튼 클릭 이벤트
void LoginDialog::OnBnClickedFindpwBtn()
{
	FindPwDlg dlg(my_odbc);
	dlg.DoModal();
}
