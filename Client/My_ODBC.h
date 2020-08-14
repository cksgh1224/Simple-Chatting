#ifndef _MY_ODBC_H_
#define _MY_ODBC_H_


#include <windows.h> // WindowsAPI (MFC) ����ϱ� ���� �߰�
#include <atlstr.h> // MFC CString ����ϱ� ���� �߰�

// ODBC API�� ����ϱ� ���� ��� ���� �߰�
#include <odbcinst.h>
#include <sqlext.h>

// ODBC API�� ����ϱ� ���� ���̺귯�� �߰�
#pragma comment(lib, "odbc32.lib")


// ����� ���
#define TR(s){CString str;str.Format(L"%s", _T(s));OutputDebugString(str);}


// ODBC_API ���̺귯��
//#ifdef _DEBUG
//	#pragma comment(lib, "D_My_ODBC.lib")
//#else
//	#pragma comment(lib, "R_My_ODBC.lib")
//#endif


// typedef �Լ� ������ : ��ȯ��(*�Լ��̸�)(�Ű�����)
// SET_RECORD_INFO    : Query�� ����� �ش��ϴ� �����͸� ���� �޸�(����� ���� ��ü)�� �����ϱ� ���� �ڵ� ����
// SQL_RESULT_RECORD  : SQL ��ɹ��� ���ؼ� ������ �����͵��� �ڽ��� ���ϴ� ���·� ��ȯ�ϰų� ����,ó���ϴ� �۾�
// ������ ȣ��Ǵ� �Լ��� ������ �۾��� �ϴ�(���̺귯���� ����ϴ�) cpp ���Ͽ� �����ؼ� ���
typedef void(*SET_RECORD_INFO)(void* ap_owner, HSTMT ah_statement, void* ap_data);
typedef int(*SQL_RESULT_RECORD)(void* ap_owner, int a_step_index, void* ap_data, ULONG a_count, unsigned short* ap_state, int option);



/*
// ex) ������ ��ü Ŭ����
class User
{
private:
	wchar_t id[20];
	wchar_t pw[20];
	wchar_t name[20];

public:
	wchar_t* GetID() { return id; }
	wchar_t* GetPW() { return pw; }
	wchar_t* GetName() { return name; }
	void SetID(wchar_t* ap_id) { wcscpy(id, ap_id); }
	void SetPW(wchar_t* ap_pw) { wcscpy(pw, ap_pw); }
	void SetName(wchar_t* ap_name) { wcscpy(name, ap_name); }
};

// Query�� ����� �ش��ϴ� �����͸� User ��ü�� �����ϱ� ���� �ڵ� ����
void SetRecordInfo(void* ap_owner, HSTMT ah_statement, void* ap_data)
{
	User* p = (User*)ap_data;

	// ���̺��� ������ �����͸� �Ӽ����� raw_data ������ �����ϱ� ���� �Ӽ����� ������ �޸� ��ġ�� ���� (������ ���۸� ��� ������ ���� ���ε�)
	SQLBindCol(ah_statement, 1, SQL_WCHAR, p->GetID(), sizeof(wchar_t) * 20, NULL);
	SQLBindCol(ah_statement, 2, SQL_WCHAR, p->GetPW(), sizeof(wchar_t) * 20, NULL);
	SQLBindCol(ah_statement, 3, SQL_WCHAR, p->GetName(), sizeof(wchar_t) * 20, NULL);
}

// SQL ��ɹ��� ���ؼ� ������ �����͵��� �ڽ��� ���ϴ� ���·� ��ȯ�ϰų� �����ϴ� �۾�
int ResultRecord(void* ap_owner, int a_step_index, void* ap_data, ULONG a_count, unsigned short* ap_state, int option)
{
	int result = 0; // ����: 1��ȯ
	CString str;
	User* p = (User*)ap_data;

	// ��ü ����� �˻�
	if (option == 0)
	{
		for (ULONG i = 0; i < a_count; i++)
		{
			if (ap_state[i] != SQL_ROW_DELETED && ap_state[i] != SQL_ROW_ERROR)
			{
				str.Format(L"ID: %s\nPW:%s\nName:%s", p->GetID(), p->GetPW(), p->GetName());
				MessageBox(NULL, str, L"�����", MB_OK);
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
}

// SET_RECORD_INFO �Լ��� ȣ��� �� ������ SetRecordInfo�Լ��� ����ȴ�

*/




class My_Odbc
{
private:
	SQLHANDLE mh_environment;  // ODBC ����� ����ϱ� ���� ȯ�� ���� (ȯ�� �ڵ�)
	SQLHDBC mh_odbc;           // ODBC �Լ��� ����ϱ� ���� ���� (���� �ڵ�)
	int m_connect_flag;        // ������ ���� ���θ� ������ ���� (1:����)
	void* mp_owner;            // ������ �ڵ��� ������ ����

public:
	My_Odbc();
	virtual ~My_Odbc();


	inline int IsConnected() { return m_connect_flag; }
	inline SQLHANDLE GetEnvHandle() { return mh_environment; }
	inline SQLHDBC GetOdbcHandle() { return mh_odbc; }


	// ������� �ʱ�ȭ
	void InitObject();

	// ���� ���� (����:1��ȯ)
	int Connect(const wchar_t* ap_dsn, const wchar_t* ap_id, const wchar_t* ap_pw, void* ap_owner = 0);

	// ���� ���� ����
	void Disconnect();


	// ��ɹ� ���� (insert, update, delete) (����: 1��ȯ)
	int ExecQuery(const wchar_t* ap_query);

	// ��ɹ� ���� (select) (����: 1��ȯ)
	int ExecQuery(const wchar_t* ap_query, int a_record_size, SET_RECORD_INFO set_record_info, SQL_RESULT_RECORD sql_result_record,
		int option, int a_record_count_per_step = 100);


};


#endif