// LoginDialog.cpp: 구현 파일
//

#include "pch.h"
#include "Client.h"
#include "LoginDialog.h"
#include "afxdialogex.h"
#include "ClientDlg.h"

// LoginDialog 대화 상자

IMPLEMENT_DYNAMIC(LoginDialog, CDialogEx)

LoginDialog::LoginDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{
	m_connect_flag = 0; // 서버와 연결 여부를 저장할 변수 (1:연결)
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LoginDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &LoginDialog::OnBnClickedButton1)
END_MESSAGE_MAP()


// LoginDialog 메시지 처리기



BOOL LoginDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ODBC를 사용하여 MySql Server 와 연결
	if (MySqlConnect() == 1)
	{
		MessageBox(L"MySql 서버 접속!",L"성공",MB_OK);
		OutputDebugString(L"MySql 서버 접속\n");
	}
	else
	{
		MessageBox(L"MySql 서버 접속 실패!", L"실패", MB_OK);
		OutputDebugString(L"MySql 서버 접속 실패\n");
	}
		




	return TRUE;
}

int LoginDialog::MySqlConnect()
{
	// ODBC 드라이버에 연결을 위한 기본 정보 설정
	SQLSetEnvAttr(NULL, SQL_ATTR_CONNECTION_POOLING, (SQLPOINTER)SQL_CP_ONE_PER_DRIVER, SQL_IS_INTEGER);

	// ODBC 기술을 사용하기 위한 환경 구성
	// 구성된 환경 정보에 대한 핸들 값은 mh_environment에 저장
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &mh_environment) != SQL_ERROR)
	{
		// 사용할 ODBC 버전 정보 설정
		SQLSetEnvAttr(mh_environment, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
		SQLSetEnvAttr(mh_environment, SQL_ATTR_CP_MATCH, (SQLPOINTER)SQL_CP_RELAXED_MATCH, SQL_IS_INTEGER);


		// ODBC 함수를 사용하기 위한 정보 구성
		// 이 정보에 대한 핸들 값은 mh_odbc에 저장
		if (SQLAllocHandle(SQL_HANDLE_DBC, mh_environment, &mh_odbc) != SQL_ERROR)
		{
			RETCODE ret_code = SQLConnect(mh_odbc,
				// 접속할 DSN 설정
				(SQLWCHAR*)L"socket_user", SQL_NTS,
				// 접속에 사용할 ID
				(SQLWCHAR*)L"root", SQL_NTS,
				// 접속에 사용할 Password
				(SQLWCHAR*)L"0000", SQL_NTS);


			if (ret_code == SQL_SUCCESS || ret_code == SQL_SUCCESS_WITH_INFO)
			{
				// ODBC를 사용하여 데이터베이스 서버에 성공적으로 접속한 경우
				m_connect_flag = 1;
				return 1;
			}
			else
			{
				// 접속에 실패한 경우, 구성했던 메모리를 제거
				if (mh_odbc != SQL_NULL_HDBC) SQLFreeHandle(SQL_HANDLE_DBC, mh_odbc);
				if (mh_environment != SQL_NULL_HENV) SQLFreeHandle(SQL_HANDLE_ENV, mh_environment);
			}
		}
	}

	return 0;
}




// '로그인' 버튼 클릭 이벤트
void LoginDialog::OnBnClickedButton1()
{
	//::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL); // 다이얼로그 종료
	//
	//// 새로운 다이얼로그 열기
	//CClientDlg dlg;
	//dlg.DoModal();	


	CString id, pw;
	GetDlgItemText(IDC_ID_EDIT, id); // Edit Control 값 가져오기
	GetDlgItemText(IDC_PW_EDIT, pw);

	OutputDebugString(L"아이디: " + id + "\n");
	OutputDebugString(L"비밀번호: " + pw + "\n");
}