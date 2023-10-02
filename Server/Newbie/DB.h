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
	// �ʱ�ȭ ���н� -1
	int init();
	// ���� ��û
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

