#pragma once
// This file defines the types used in ODBC
#include <sqltypes.h>

//  This is the the main include for ODBC Core functions.
#include <sql.h>

// This is the include for applications using the Microsoft SQL Extensions
#include <sqlext.h>



class DB
{
public:
	DB();
	~DB();
	// 초기화 실패시 -1
	int init();
	// 쿼리 요청
	void requestQuery(SQLWCHAR query[]);
	UINT32 getState() { return state; };

private:
	SQLHENV henv = nullptr;
	SQLHDBC hdbc = nullptr;
	SQLHSTMT hstmt = nullptr;
	

	// state table
	enum : UINT32
	{
		DBOFF,
		DBON,
		DBERROR,

	};
	// no thread safety
	UINT32 state = DBOFF;
};

