#pragma once


// CreateAccountDlg 대화 상자

class CreateAccountDlg : public CDialogEx
{
private:
	My_Odbc my_odbc;
	bool id_check;  // 사용 가능한 아이디인지 체크할 변수 (아이디 중복확인)

	DECLARE_DYNAMIC(CreateAccountDlg)

public:
	CreateAccountDlg(My_Odbc odbc, CWnd* pParent = nullptr);   // 생성자에서 MyOdbc 객체를 넘겨받아 my_odbc에 저장한다
	virtual CreateAccountDlg::~CreateAccountDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATEACCOUNT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddBtn(); // 회원가입 버튼 이벤트
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedIdcheckBtn();
};
