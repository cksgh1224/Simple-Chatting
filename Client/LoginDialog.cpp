// LoginDialog.cpp: 구현 파일
//

#include "pch.h"
#include "Client.h"
#include "LoginDialog.h"
#include "afxdialogex.h"
#include "ClientDlg.h"
#include "CreateAccountDlg.h"

// LoginDialog 대화 상자

IMPLEMENT_DYNAMIC(LoginDialog, CDialogEx)

LoginDialog::LoginDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_DIALOG, pParent)
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
	ON_BN_CLICKED(IDC_LOGIN_BTN, &LoginDialog::OnBnClickedLoginBtn)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SELECT_BTN, &LoginDialog::OnBnClickedSelectBtn)
	ON_BN_CLICKED(IDC_JOIN_BTN, &LoginDialog::OnBnClickedJoinBtn)
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


// ODBC를 사용하여 MySql Server 와 연결
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


// WM_DESTROY 메시지 처리
void LoginDialog::OnDestroy()
{
	CDialogEx::OnDestroy();

	// ODBC를 위해 사용하던 핸들 값, 구성했던 메모리 제거
	// 서버에 연결되어 있는 경우에만 제거 (서버에 연결을 실패했다면 실패한 시점에서 정리됨)
	if (m_connect_flag == 1)
	{
		if (mh_odbc != SQL_NULL_HDBC) SQLFreeHandle(SQL_HANDLE_DBC, mh_odbc);
		if (mh_environment != SQL_NULL_HENV) SQLFreeHandle(SQL_HANDLE_ENV, mh_environment);
	}

}




// '로그인' 버튼 클릭 이벤트
void LoginDialog::OnBnClickedLoginBtn()
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


	/// <summary>
	
	/// </summary>
	unsigned short record_state; // 읽어온 데이터의 상태를 기록할 변수
	unsigned long record_num = 0;           // 읽은 데이터의 개수를 저장할 변수
	_UserData raw_data;          // 읽어온 데이터를 저장할 변수

	CString query_str;
	query_str.Format(L"select mid,mpw,mname from user where mid='%s' and mpw='%s' ", id, pw);


	HSTMT h_statement;


	// 쿼리문을 위한 메모리 할당
	if (SQLAllocHandle(SQL_HANDLE_STMT, mh_odbc, &h_statement) == SQL_SUCCESS)
	{
		// Qoery 문을 수행할 때 타임 아웃 설정
		SQLSetStmtAttr(h_statement, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)15, SQL_IS_UINTEGER);

		// 가져온 데이터를 저장할 메모리 크기 설정
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)sizeof(raw_data), 0);

		// 데이터를 가져올때 동시성에 대한 방식 설정
		SQLSetStmtAttr(h_statement, SQL_ATTR_CONCURRENCY, (SQLPOINTER)SQL_CONCUR_ROWVER, SQL_IS_UINTEGER);

		// 검색된 데이터의 위치를 기억하는 방식 설정
		SQLSetStmtAttr(h_statement, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_KEYSET_DRIVEN, SQL_IS_UINTEGER);

		// 데이터를 가져오는 최대 단위 설정
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROW_NUMBER, (SQLPOINTER)MAX_COUNT, SQL_IS_UINTEGER);

		// 읽은 데이터의 상태를 저장할 변수의 주소를 전달
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROW_STATUS_PTR, &record_state, 0);

		// 읽은 데이터의 개수를 저장할 변수의 주소를 전달
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROWS_FETCHED_PTR, &record_num, 0);


		// 테이블에서 가져온 데이터를 속성별로 raw_data 변수에 저장하기 위해 속성별로 저장할 메모리 위치를 설정
		SQLBindCol(h_statement, 1, SQL_WCHAR, raw_data.id, sizeof(wchar_t) * 20, NULL);
		SQLBindCol(h_statement, 2, SQL_WCHAR, raw_data.pw, sizeof(wchar_t) * 20, NULL);
		SQLBindCol(h_statement, 3, SQL_WCHAR, raw_data.name, sizeof(wchar_t) * 20, NULL);


		// SQL 명령문 실행
		//RETCODE ret_code = SQLExecDirect(h_statement, (SQLWCHAR*)query, SQL_NTS);
		RETCODE ret_code = SQLExecDirect(h_statement, (SQLWCHAR*)(const wchar_t*)query_str, SQL_NTS);
		
		if (ret_code = (SQLFetchScroll(h_statement, SQL_FETCH_NEXT, 0) != SQL_NO_DATA))
		{
			if (record_state != SQL_ROW_DELETED && record_state != SQL_ROW_ERROR)
			{
				MessageBox(L"로그인 성공!", NULL, MB_OK);
			}
		}
		else
		{
			MessageBox(L"입력 정보가 다릅니다!", NULL, MB_OK);
		}

		// Query 문을 위해 할당할 메모리 해제
		SQLFreeHandle(SQL_HANDLE_STMT, h_statement);
	}


} // end





// '검색' 버튼 클릭 이벤트
void LoginDialog::OnBnClickedSelectBtn()
{
	// user 테이블에 저장된 데이터를 읽을 SQL문
	wchar_t query[256] = L"select mid,mpw,mname from user;";
	SelectQuery(query);
}

void LoginDialog::SelectQuery(wchar_t* query)
{
	unsigned short record_state[MAX_COUNT]; // 읽어온 데이터의 상태를 기록할 변수
	unsigned long record_num = 0;           // 읽은 데이터의 개수를 저장할 변수
	_UserData raw_data[MAX_COUNT];          // 읽어온 데이터를 저장할 변수
	CString str;                            // 읽은 데이터로 문자열 구성
	
	// 데이터를 저장할 배열을 초기화
	memset(raw_data, 0, sizeof(raw_data));

	HSTMT h_statement;

	// 쿼리문을 위한 메모리 할당
	if (SQLAllocHandle(SQL_HANDLE_STMT, mh_odbc, &h_statement) == SQL_SUCCESS)
	{
		// Qoery 문을 수행할 때 타임 아웃 설정
		SQLSetStmtAttr(h_statement, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)15, SQL_IS_UINTEGER);

		// 가져온 데이터를 저장할 메모리 크기 설정
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)sizeof(raw_data), 0);

		// 데이터를 가져올때 동시성에 대한 방식 설정
		SQLSetStmtAttr(h_statement, SQL_ATTR_CONCURRENCY, (SQLPOINTER)SQL_CONCUR_ROWVER, SQL_IS_UINTEGER);

		// 검색된 데이터의 위치를 기억하는 방식 설정
		SQLSetStmtAttr(h_statement, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_KEYSET_DRIVEN, SQL_IS_UINTEGER);

		// 데이터를 가져오는 최대 단위 설정
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROW_NUMBER, (SQLPOINTER)MAX_COUNT, SQL_IS_UINTEGER);

		// 읽은 데이터의 상태를 저장할 변수의 주소를 전달
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROW_STATUS_PTR, record_state, 0);

		// 읽은 데이터의 개수를 저장할 변수의 주소를 전달
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROWS_FETCHED_PTR, &record_num, 0);


		// 테이블에서 가져온 데이터를 속성별로 raw_data 변수에 저장하기 위해 속성별로 저장할 메모리 위치를 설정
		SQLBindCol(h_statement, 1, SQL_WCHAR, raw_data[0].id, sizeof(wchar_t) * 20, NULL);
		SQLBindCol(h_statement, 2, SQL_WCHAR, raw_data[0].pw, sizeof(wchar_t) * 20, NULL);
		SQLBindCol(h_statement, 3, SQL_WCHAR, raw_data[0].name, sizeof(wchar_t) * 20, NULL);


		// SQL 명령문 실행
		SQLExecDirect(h_statement, (SQLWCHAR*)query, SQL_NTS);

		while (SQLFetchScroll(h_statement, SQL_FETCH_NEXT, 0) != SQL_NO_DATA)
		{
			// 데이터 개수만큼 반복하면서 작업한다
			for (int i = 0; i < record_num; i++)
			{
				// 가져온 데이터가 삭제된 정보나 에러가 아니라면 문자열 구성
				if (record_state[i] != SQL_ROW_DELETED && record_state[i] != SQL_ROW_ERROR)
				{
					str.Format(L"ID: %s, PW: %s, NAME: %s", raw_data[i].id, raw_data[i].pw, raw_data[i].name);
				}
				MessageBox(str, L"회원", MB_OK);
			}
		}

		// Query 문을 위해 할당할 메모리 해제
		SQLFreeHandle(SQL_HANDLE_STMT, h_statement);
	}
}


// '회원가입' 버튼 클릭 이벤트
void LoginDialog::OnBnClickedJoinBtn()
{
	//CreateAccountDlg dlg;
	CreateAccountDlg dlg(mh_environment, mh_odbc);
	dlg.DoModal();	

}

