// FindPwDlg.cpp: 구현 파일
//

#include "pch.h"
#include "Client.h"
#include "FindPwDlg.h"
#include "afxdialogex.h"


// FindPwDlg 대화 상자

IMPLEMENT_DYNAMIC(FindPwDlg, CDialogEx)

FindPwDlg::FindPwDlg(My_Odbc odbc, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FINDPW_DIALOG, pParent)
{
	my_odbc = odbc;
}

FindPwDlg::~FindPwDlg()
{
}

void FindPwDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(FindPwDlg, CDialogEx)
	ON_BN_CLICKED(IDC_FINDPW_BTN, &FindPwDlg::OnBnClickedFindpwBtn)
END_MESSAGE_MAP()


// FindPwDlg 메시지 처리기


// 다이얼로그 엔터 종료 막기
BOOL FindPwDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN) return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}



// '비밀번호 찾기' 버튼 클릭 이벤트
void FindPwDlg::OnBnClickedFindpwBtn()
{
	CString id, ip, query;
	GetDlgItemText(IDC_ID_EDIT, id);
	GetDlgItemText(IDC_IP_EDIT, ip);

	if (id.IsEmpty() || ip.IsEmpty())
	{
		MessageBox(L"아이디, IP주소를 입력해 주세요", NULL, MB_OK);
		return;
	}

	query.Format(L" select mid, mpw, mname from user where mid='%s' and mip='%s' ", id, ip);

	if (my_odbc.ExecQuery(query, sizeof(UserAccount), SetRecordInfo, ResultRecord, 3))
	{
		::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL); // 대화상자 종료
	}
	else
	{
		MessageBox(L"입력하신 정보가 일치하지 않습니다", L"실패", MB_OK);
	}
}
