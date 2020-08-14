#pragma once


// FindIdDlg 대화 상자

class FindIdDlg : public CDialogEx
{
private:
	My_Odbc my_odbc;    // ODBC를 사용하기 위한 객체

	DECLARE_DYNAMIC(FindIdDlg)

public:
	FindIdDlg(My_Odbc odbc, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~FindIdDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FINDID_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedFindidBtn();
};
