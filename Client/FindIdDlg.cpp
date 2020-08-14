// FindIdDlg.cpp: 구현 파일
//

#include "pch.h"
#include "Client.h"
#include "FindIdDlg.h"
#include "afxdialogex.h"


// FindIdDlg 대화 상자

IMPLEMENT_DYNAMIC(FindIdDlg, CDialogEx)

FindIdDlg::FindIdDlg(My_Odbc odbc, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FINDID_DIALOG, pParent)
{
	my_odbc = odbc;
}

FindIdDlg::~FindIdDlg()
{
}

void FindIdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(FindIdDlg, CDialogEx)
	ON_BN_CLICKED(IDC_FINDID_BTN, &FindIdDlg::OnBnClickedFindidBtn)
END_MESSAGE_MAP()


// FindIdDlg 메시지 처리기


// 다이얼로그 엔터 종료 막기
BOOL FindIdDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN) return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


// '아이디 찾기' 버튼 클릭 이벤트
void FindIdDlg::OnBnClickedFindidBtn()
{
	
	CString name, nickname, query;
	GetDlgItemText(IDC_NAME_EDIT, name);
	GetDlgItemText(IDC_NICKNAME_EDIT, nickname);

	if (name.IsEmpty() || nickname.IsEmpty())
	{
		MessageBox(L"이름, 닉네임을 입력해 주세요", NULL, MB_OK);
		return;
	}

	query.Format(L" select mid, mpw, mname from user where mname='%s' and mnickname='%s' ", name, nickname);

	if (my_odbc.ExecQuery(query, sizeof(UserAccount), SetRecordInfo, ResultRecord, 2))
	{
		::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL); // 대화상자 종료
	}
	else
	{
		MessageBox(L"입력하신 정보가 일치하지 않습니다", L"실패", MB_OK);
	}

}
