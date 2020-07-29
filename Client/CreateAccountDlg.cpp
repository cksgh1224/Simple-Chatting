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

	query.Format(L"insert into user values('%s', '%s', '%s', '%s', '%s')", id, pw, name, ip_address, nickname);
	
}
