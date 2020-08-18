// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일

#include "pch.h"

// 미리 컴파일된 헤더를 사용하는 경우 컴파일이 성공하려면 이 소스 파일이 필요합니다.



// 데이터 객체 -> SocketAPI -> UserAccount class


UserAccount select_data; // ResultRecord 에서 Select 문의 결과를 저장할 객체 (전역변수 - 사용하는 파일에서 extern 으로 선언해서 사용)
// static: 전역범위의 변수에 static이 사용된경우, 해당 파일 내부에서는 전역변수처럼 사용되지만 다른 소스파일에서는 참조할 수 없음


// Query의 결과에 해당하는 데이터를 User 객체와 연결하기 위한 코드 구성
void SetRecordInfo(void* ap_owner, HSTMT ah_statement, void* ap_data)
{	
	UserAccount* p = (UserAccount*)ap_data;

	// 테이블에서 가져온 데이터를 속성별로 raw_data 변수에 저장하기 위해 속성별로 저장할 메모리 위치를 설정 (데이터 버퍼를 결과 집합의 열에 바인딩)
	SQLBindCol(ah_statement, 1, SQL_WCHAR, p->GetID(), sizeof(wchar_t) * 20, NULL);
	SQLBindCol(ah_statement, 2, SQL_WCHAR, p->GetPW(), sizeof(wchar_t) * 20, NULL);
	SQLBindCol(ah_statement, 3, SQL_WCHAR, p->GetName(), sizeof(wchar_t) * 20, NULL);
	SQLBindCol(ah_statement, 4, SQL_WCHAR, p->GetIP(), sizeof(wchar_t) * 32, NULL);
	SQLBindCol(ah_statement, 6, SQL_WCHAR, p->GetNickName(), sizeof(wchar_t) * 32, NULL);
	// 5 : mregdata (가입일)
}


// SQL 명령문에 의해서 가져온 데이터들을 자신이 원하는 형태로 변환하거나 저장하는 작업
// ap_owner: 윈도우 핸들, ap_data: 읽은 데이터, a_step_index: 0부터 시작해서 데이터를 하나 읽을때마다 1씩 증가
// a_count: 읽은 데이터 개수, ap_state: 읽어온 데이터의 상태, option: 검색 조건
int ResultRecord(void* ap_owner, int a_step_index, void* ap_data, ULONG a_count, unsigned short* ap_state, int option)
{
	int result = 0; // 성공: 1반환

	CString str;
	UserAccount* p = (UserAccount*)ap_data;
	

	// 검색 결과를 user에 저장
	select_data.SetID(p->GetID());
	select_data.SetPW(p->GetPW());
	select_data.SetName(p->GetName());
	select_data.SetIP(p->GetIP());
	select_data.SetNickName(p->GetNickName());


	// 전체 사용자 검색
	if (option == 0)
	{
		for (ULONG i = 0; i < a_count; i++)
		{
			if (ap_state[i] != SQL_ROW_DELETED && ap_state[i] != SQL_ROW_ERROR)
			{
				str.Format(L"ID: %s\nPW:%s\nName:%s", p->GetID(), p->GetPW(), p->GetName());
				MessageBox(NULL, str, L"사용자", MB_OK);
				result = 1;
			}
			else
			{
				MessageBox(NULL, L"ap_state[0] : SQL_ROW_DELETED or SQL_ROW_ERROR", NULL, MB_OK);
				result = 0;
			}
			p++;
			ap_state++;
		}
	}

	// 로그인 체크
	else if (option == 1)
	{
		if (ap_state[0] != SQL_ROW_DELETED && ap_state[0] != SQL_ROW_ERROR)
		{
			//MessageBox(NULL, L"로그인 성공", NULL, MB_OK);
			result = 1;
		}
		else
		{
			MessageBox(NULL, L"ap_state[0] : SQL_ROW_DELETED or SQL_ROW_ERROR", NULL, MB_OK);
			result = 0;
		}
	}

	// 아이디 찾기
	else if (option == 2)
	{
		if (ap_state[0] != SQL_ROW_DELETED && ap_state[0] != SQL_ROW_ERROR)
		{
			str.Format(L"%s님의 아이디: %s", p->GetName(), p->GetID());
			MessageBox(NULL, str, NULL, MB_OK);
			result = 1;
		}
		else
		{
			MessageBox(NULL, L"ap_state[0] : SQL_ROW_DELETED or SQL_ROW_ERROR", NULL, MB_OK);
			result = 0;
		}
	}

	// 비밀번호 찾기
	else if (option == 3)
	{
		if (ap_state[0] != SQL_ROW_DELETED && ap_state[0] != SQL_ROW_ERROR)
		{
			str.Format(L"%s님의 비밀번호: %s", p->GetName(), p->GetPW());
			MessageBox(NULL, str, NULL, MB_OK);
			result = 1;
		}
		else
		{
			MessageBox(NULL, L"ap_state[0] : SQL_ROW_DELETED or SQL_ROW_ERROR", NULL, MB_OK);
			result = 0;
		}
	}

	// 아이디 중복확인
	else if (option == 4)
	{
		if (ap_state[0] != SQL_ROW_DELETED && ap_state[0] != SQL_ROW_ERROR)
		{
			// 입력한 아이디가 검색이 됬으면 (db데이터에 있으면 -> 이미 있는 아이디 -> 아이디 사용 불가)
			//str.Format(L"%s 는 사용할수 없는 아이디 입니다", p->GetID());
			//MessageBox(NULL, str, NULL, MB_OK);
			result = 1;
		}
		else
		{
			MessageBox(NULL, L"ap_state[0] : SQL_ROW_DELETED or SQL_ROW_ERROR", NULL, MB_OK);
			result = 0;
		}
	}

	return result;
}