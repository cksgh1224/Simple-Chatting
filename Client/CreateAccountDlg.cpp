// CreateAccountDlg.cpp: 구현 파일
//

#include "pch.h"
#include "Client.h"
#include "CreateAccountDlg.h"
#include "afxdialogex.h"


// CreateAccountDlg 대화 상자

IMPLEMENT_DYNAMIC(CreateAccountDlg, CDialogEx)

CreateAccountDlg::CreateAccountDlg(My_Odbc odbc, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_JOIN_DIALOG, pParent)
{
	my_odbc = odbc;
	id_check = false;
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
	ON_BN_CLICKED(IDC_IDCHECK_BTN, &CreateAccountDlg::OnBnClickedIdcheckBtn)
END_MESSAGE_MAP()


// CreateAccountDlg 메시지 처리기



// 다이얼로그 엔터 종료 막기
BOOL CreateAccountDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN) return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}



// '회원가입' 버튼 이벤트
void CreateAccountDlg::OnBnClickedAddBtn()
{
	if (!id_check) // 중복확인을 하지 않았으면
	{
		MessageBox(L"아이디 중복확인을 해주세요", NULL, MB_OK);
		return;
	}

	CString id, pw, name, nickname, ip, query;
	GetDlgItemText(IDC_ID_EDIT, id);
	GetDlgItemText(IDC_PW_EDIT, pw);
	GetDlgItemText(IDC_NAME_EDIT, name);
	GetDlgItemText(IDC_NICKNAME_EDIT, nickname);
	GetDlgItemText(IDC_IPADDRESS_EDIT, ip);

	if (id.IsEmpty() || pw.IsEmpty() || ip.IsEmpty() || name.IsEmpty() || nickname.IsEmpty())
	{
		MessageBox(L"사용자 정보를 모두 입력해주세요", NULL, MB_OK);
		return;
	}

	query.Format(L" insert into user (mid,mpw,mname,mip,mnickname) values ('%s','%s','%s','%s','%s') ", id, pw, name, ip, nickname);

	if (my_odbc.ExecQuery(query))
	{
		MessageBox(L"회원가입 성공", NULL, MB_OK);
		::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);  // 대화상자 종료
	}
	else
	{
		MessageBox(L"회원가입 실패", NULL, MB_OK);
	}

}



// '중복확인' 버튼 클릭 이벤트
// 사용 가능한 아이디 라면 id_check를 true로 바꾸고 IDC_ID_EDIT를 비활성화 시킨다, 중복확인 버튼의 텍스트를 다시입력으로 바꾼다
// 다른 아이디를 사용하고 싶으면 다시입력 클릭 -> id_check를 false로 바꾸고 IDC_ID_EDIT를 활성화 시킨다, 다시입력 버튼의 텍스트를 중복확인으로 바꾼다
void CreateAccountDlg::OnBnClickedIdcheckBtn()
{
	// 중복확인 이후 버튼을 다시 누르면 (아이디 다시 입력)
	if (id_check)
	{
		id_check = false;
		GetDlgItem(IDC_ID_EDIT)->EnableWindow(TRUE);   // EditControl 활성화
		GetDlgItem(IDC_ID_EDIT)->SetWindowTextW(L"");  // caption 변경 (빈문자열로 초기화)
		GetDlgItem(IDC_IDCHECK_BTN)->SetWindowTextW(L"중복확인");
		return;
	}
	
	
	CString id, query;
	GetDlgItemText(IDC_ID_EDIT, id);

	if (id.IsEmpty())
	{
		MessageBox(L"아이디를 입력해 주세요", NULL, MB_OK);
		return;
	}

	query.Format(L" select mid from user where mid='%s' ", id);

	// ExecQuery가 성공 -> 입력한 아이디 검색 성공 -> 이미 있는 아이디 -> 아이디 사용 불가
	if (my_odbc.ExecQuery(query, sizeof(UserAccount), SetRecordInfo, ResultRecord, 4))
	{
		id_check = false;
		MessageBox(id + L" 사용할 수 없는 아이디 입니다", NULL, MB_OK);
	}
	else // 아이디 사용 가능
	{
		id_check = true;
		MessageBox(id + L" 사용 가능한 아이디 입니다", NULL, MB_OK);
		GetDlgItem(IDC_ID_EDIT)->EnableWindow(FALSE);            // EditControl 비활성화
		GetDlgItem(IDC_IDCHECK_BTN)->SetWindowTextW(L"다시입력"); // caption 변경
	}

}
