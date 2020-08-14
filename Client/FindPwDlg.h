#pragma once


// FindPwDlg 대화 상자

class FindPwDlg : public CDialogEx
{
private:
	My_Odbc my_odbc;

	DECLARE_DYNAMIC(FindPwDlg)

public:
	FindPwDlg(My_Odbc odbc, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~FindPwDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FINDPW_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedFindpwBtn();
};
