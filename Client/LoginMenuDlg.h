#pragma once

extern UserAccount select_data; // pch.cpp 에 선언된 select_data (전역변수)를 사용하기 위해 extern 선언

// LoginMenuDlg 대화 상자

class LoginMenuDlg : public CDialogEx
{
private:
	UserAccount login_data; // select 문의 결과를 저장할 객체

	DECLARE_DYNAMIC(LoginMenuDlg)

public:
	LoginMenuDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~LoginMenuDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGINMENU_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedDeleteBtn();
};
