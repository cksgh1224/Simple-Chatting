#pragma once


// ODBC API를 사용하기 위한 헤더 파일 추가
#include <odbcinst.h>
#include <sqlext.h>

// ODBC API를 사용하기 위한 라이브러리 추가
#pragma comment(lib, "odbc32.lib")



// LoginDialog 대화 상자

class LoginDialog : public CDialogEx
{
private:
	SQLHANDLE mh_environment;  // ODBC 기술을 사용하기 위한 환경 정보
	SQLHDBC mh_odbc;           // ODBC 함수를 사용하기 위한 정보
	int m_connect_flag;        // 서버와 연결 여부를 저장할 변수 (1:연결)


	DECLARE_DYNAMIC(LoginDialog)

public:
	LoginDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~LoginDialog();

	int MySqlConnect(); // ODBC를 사용하여 MySql Server 와 연결


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
};
