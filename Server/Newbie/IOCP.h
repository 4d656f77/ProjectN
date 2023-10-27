#pragma once
class IOCP
{
public:
	IOCP()
	{ 
		// �ʱ�ȭ ���н� ��� ������ ���ΰ�?
		HIOCP = CreateNewIoCompletionPort(0);
		
	};
	~IOCP() { CloseHandle(HIOCP); };

	HANDLE* getHendle(void) { return &HIOCP; };

	HANDLE CreateNewIoCompletionPort(DWORD dwNumberOfConcurrentThreads);


	BOOL AssociateDeviceWithCompletionPort(HANDLE hExistingCompletionPort, HANDLE hFileHandle, ULONG_PTR ulpCompletionKey);


private:
	HANDLE HIOCP = nullptr;
};

