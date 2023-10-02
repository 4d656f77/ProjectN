#include "pch.h"
#include "DB.h"


DB::DB()
{
	if (init() == -1) state = DBERROR;
}

DB::~DB()
{
	// 리소스 해제
	if (hstmt) {
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		hstmt = nullptr;
	}
	// DB 연결 해제, 핸들 해제하기
	if (hdbc) {
		SQLDisconnect(hdbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
		hdbc = nullptr;
	}
	// 환경 핸들 해제하기
	if (henv)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
		henv = nullptr;
	}
}

int DB::init()
{
	SQLRETURN ret = NULL;
	// 환경 핸들 할당하기
	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (ret == SQL_ERROR || ret == SQL_INVALID_HANDLE)
	{
		std::wcout << L"henv : SQLAllocHandle - SQL_INVALID_HANDLE or SQL_ERROR.\n";
		return -1;
	}

	// ODBC 버전 설정
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	if (ret == SQL_ERROR || ret == SQL_INVALID_HANDLE)
	{
		std::wcout << L"SQLSetEnvAttr - SQL_ERROR or SQL_INVALID_HANDLE\n";
		return -1;
	}
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	if (ret == SQL_ERROR || ret == SQL_INVALID_HANDLE)
	{
		std::wcout << L"hdbc : SQLAllocHandle - SQL_INVALID_HANDLE or SQL_ERROR.\n";
		return -1;
	}
	// 연결 문자열 만들기
	// DSN 이용해서 연결하기
	SQLWCHAR* connectionString = (SQLWCHAR*)L"DRIVER={SQL Server};SERVER=localhost, 1433;DSN=newbieDB;UID=sa;PWD=test;";

	// DB에 연결
	ret = SQLDriverConnectW(hdbc, NULL, connectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
	if (ret == SQL_ERROR || ret == SQL_INVALID_HANDLE || ret == SQL_STILL_EXECUTING)
	{
		std::wcout << L"SQLDriverConnectW - SQL_ERROR, SQL_INVALID_HANDLE, or SQL_STILL_EXECUTING.\n";
		return -1;
	}
	// 명령문 핸들 생성
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	if (ret == SQL_ERROR || ret == SQL_INVALID_HANDLE)
	{
		std::wcout << L"hstmt : SQLAllocHandle - SQL_INVALID_HANDLE or SQL_ERROR.\n";
		return -1;
	}
	// 초기화 성공
	return 0;
}

void DB::requestQuery(SQLWCHAR query[])
{
	
	SQLRETURN ret = SQLExecDirectW(hstmt, query, SQL_NTS);

	// 결과 출력
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		SQLINTEGER ID;
		SQLWCHAR Password[50];
		while (SQLFetch(hstmt) == SQL_SUCCESS)
		{
			SQLGetData(hstmt, 1, SQL_C_LONG, &ID, sizeof(ID), NULL);
			SQLGetData(hstmt, 2, SQL_C_WCHAR, Password, 50, NULL);
			std::wcout << L"ID : " << ID << L"| Password : " << Password << L"\n";
		}
	}
}
