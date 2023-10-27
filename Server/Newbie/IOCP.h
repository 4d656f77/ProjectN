#pragma once
class IOCP
{
public:
	IOCP()
	{ 
		// 초기화 실패시 어떻게 대응할 것인가?
		HIOCP = CreateNewIoCompletionPort(0);
		
	};
	~IOCP() { CloseHandle(HIOCP); };

	HANDLE* getHendle(void) { return &HIOCP; };

	HANDLE CreateNewIoCompletionPort(DWORD dwNumberOfConcurrentThreads);


	BOOL AssociateDeviceWithCompletionPort(HANDLE hExistingCompletionPort, HANDLE hFileHandle, ULONG_PTR ulpCompletionKey);


private:
	HANDLE HIOCP = nullptr;
};

