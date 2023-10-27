#include "pch.h"
#include "IOCP.h"

HANDLE IOCP::CreateNewIoCompletionPort(DWORD dwNumberOfConcurrentThreads)
{
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
}

BOOL IOCP::AssociateDeviceWithCompletionPort(HANDLE hExistingCompletionPort, HANDLE hFileHandle, ULONG_PTR ulpCompletionKey)
{
	HANDLE _HIOCP = CreateIoCompletionPort(hFileHandle, hExistingCompletionPort, ulpCompletionKey, 0);
	return (_HIOCP == hExistingCompletionPort);
}
