#pragma once


// CreateAccountDlg 대화 상자

class CreateAccountDlg : public CDialogEx
{
private:
	SQLHANDLE mh_environment;  // ODBC 기술을 사용하기 위한 환경 정보
	SQLHDBC mh_odbc;           // ODBC 함수를 사용하기 위한 정보

	DECLARE_DYNAMIC(CreateAccountDlg)

public:
	CreateAccountDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	CreateAccountDlg(SQLHANDLE environment, SQLHDBC odbc, CWnd* pParent = nullptr); 
	virtual ~CreateAccountDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JOIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddBtn(); // 회원가입 버튼 이벤트
	void InsertQuery(CString& query); // Insert 문 수행
};
