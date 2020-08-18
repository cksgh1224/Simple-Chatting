#pragma once


extern UserAccount select_data; // pch.cpp 에 선언된 select_data (전역변수)를 사용하기 위해 extern 선언

// LoginDialog 대화 상자

class LoginDialog : public CDialogEx
{
private:
	My_Odbc my_odbc;    // ODBC를 사용하기 위한 객체
	
	UserAccount login_data;     // select 문의 결과를 저장할 객체

	DECLARE_DYNAMIC(LoginDialog)

public:
	LoginDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~LoginDialog();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoginBtn();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedSelectBtn();
	afx_msg void OnBnClickedJoinBtn();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedFindidBtn();
	afx_msg void OnBnClickedFindpwBtn();
};