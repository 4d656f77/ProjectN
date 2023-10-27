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

	HANDLE CreateNewIoCompletionPort(DWORD dwNumberOfConcurrentThreads)
	{
		return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
	}

	BOOL AssociateDeviceWithCompletionPort(HANDLE hExistingCompletionPort, HANDLE hFileHandle, ULONG_PTR ulpCompletionKey)
	{
		HANDLE _HIOCP = CreateIoCompletionPort(hFileHandle, hExistingCompletionPort, ulpCompletionKey, 0);

		return (_HIOCP == hExistingCompletionPort);
	}


private:
	HANDLE HIOCP = nullptr;
};

