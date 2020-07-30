// CreateAccountDlg.cpp: 구현 파일
//

#include "pch.h"
#include "Client.h"
#include "CreateAccountDlg.h"
#include "afxdialogex.h"


// CreateAccountDlg 대화 상자

IMPLEMENT_DYNAMIC(CreateAccountDlg, CDialogEx)

CreateAccountDlg::CreateAccountDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_JOIN_DIALOG, pParent)
{

}
CreateAccountDlg::CreateAccountDlg(SQLHANDLE environment, SQLHDBC odbc, CWnd* pParent) : CDialogEx(IDD_JOIN_DIALOG, pParent)
{
	mh_environment = environment;
	mh_odbc = odbc;
}



CreateAccountDlg::~CreateAccountDlg()
{
}

void CreateAccountDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CreateAccountDlg, CDialogEx)
	ON_BN_CLICKED(IDC_ADD_BTN, &CreateAccountDlg::OnBnClickedAddBtn)
END_MESSAGE_MAP()


// CreateAccountDlg 메시지 처리기



// '회원가입' 버튼 이벤트
void CreateAccountDlg::OnBnClickedAddBtn()
{
	CString query, id, pw, name, ip_address, nickname;

	GetDlgItemText(IDC_ID_EDIT, id);
	GetDlgItemText(IDC_PW_EDIT, pw);
	GetDlgItemText(IDC_IPADDRESS_EDIT, ip_address);
	GetDlgItemText(IDC_NICKNAME_EDIT, nickname);

	query.Format(L"insert into user (mid,mpw,mname,mip,mnickname) values('%s', '%s', '%s', '%s', '%s')",
		id, pw, name, ip_address, nickname);
	
	InsertQuery(query);

}


void CreateAccountDlg::InsertQuery(CString& query)
{
	SQLHSTMT h_statement;

	// 쿼리문을 위한 메모리 할당
	if (SQLAllocHandle(SQL_HANDLE_STMT, mh_odbc, &h_statement) == SQL_SUCCESS)
	{
		// Qoery 문을 수행할 때 타임 아웃 설정
		SQLSetStmtAttr(h_statement, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)15, SQL_IS_UINTEGER);

		// SQL 명령문 실행
		RETCODE ret = SQLExecDirect(h_statement, (SQLWCHAR*)(const wchar_t*)query, SQL_NTS);

		// 성공적으로 완료되었는지 체크
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) MessageBox(L"회원가입 성공", NULL, MB_OK);
		else MessageBox(L"회원가입 실패", NULL, MB_OK);

		// Commit (명령 수행이 완료되었다는 것을 설정)
		SQLEndTran(SQL_HANDLE_ENV, mh_environment, SQL_COMMIT);

		// 쿼리문을 위해 할당한 메모리 해제
		SQLFreeHandle(SQL_HANDLE_STMT, h_statement);
	}
	else MessageBox(L"쿼리문을 위한 메모리 할당 실패", NULL, MB_OK);
}
