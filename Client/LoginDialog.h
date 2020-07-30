#pragma once


// pch.h 에 선언
//// ODBC API를 사용하기 위한 헤더 파일 추가
//#include <odbcinst.h>
//#include <sqlext.h>
//
//// ODBC API를 사용하기 위한 라이브러리 추가
//#pragma comment(lib, "odbc32.lib")


// 데이터를 가져오는 최대 단위
#define MAX_COUNT 100

// 테이블에 저장된 데이터 한 개를 저장할 구조체
struct _UserData
{
	wchar_t id[20];
	wchar_t pw[20];
	wchar_t name[20];
};


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
	void SelectQuery(wchar_t* query);


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
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
};
