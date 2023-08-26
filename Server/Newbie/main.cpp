#include "pch.h"


HANDLE CreateNewIoCompletionPort(DWORD dwNumberOfConcurrentThreads)
{
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
}

BOOL AssociateDeviceWithCompletionPort(HANDLE hExistingCompletionPort, HANDLE hFileHandle, ULONG_PTR ulpCompletionKey)
{
	HANDLE _HIOCP = CreateIoCompletionPort(hFileHandle, hExistingCompletionPort, ulpCompletionKey, 0);

	return (_HIOCP == hExistingCompletionPort);
}





int main()
{
	WSADATA wsaData;


	int iResult;
	// The WSAStartup function initiates use of the Winsock DLL by a process.
	// 2.2 버전 사용
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cout << "WSAStartup failed : " << iResult << std::endl;
		return 1;
	}

	// 소켓에 등록할 정보를 초기화
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(7777);



	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	iResult = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// IOCP 생성
	HANDLE HIOCP = CreateNewIoCompletionPort(0);

	// 클라이언트 정보
	SOCKADDR_IN clientAddr;
	ZeroMemory(&clientAddr, sizeof(clientAddr));
	int clientAddrLen = sizeof(clientAddr);

	//							구조체 차이점
	//		SOCKADDR_IN								SOCKADDR
	//     USHORT sin_port;  2BYTE	 			CHAR sa_data[14];
	//     IN_ADDR sin_addr; 4BYTE					
	//     CHAR sin_zero[8]; 8BYTE					

	SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "acceptEx failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// IOCP에 클라이언트 소켓 등록
	
	if (AssociateDeviceWithCompletionPort(HIOCP, (HANDLE)clientSocket, (ULONG_PTR)clientSocket) == false)
	{
		std::cout << "AssociateDeviceWithCompletionPort failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}


	while (true)
	{
		// IOCP에 등록한 소켓의 api 결과를 확인하기 위해서 OVERLLAPED 구조체가 필요하다.

		OVERLAPPED overlapped;
		ZeroMemory(&overlapped, sizeof(overlapped));


		// Recv를 받기위한 버퍼길이 + 버퍼포인터
		CHAR recvBuffer[100] = {0};
		WSABUF wsaBuf;
		wsaBuf.buf = recvBuffer;
		wsaBuf.len = sizeof(recvBuffer);


		// dwBufferCount : The number of WSABUF structures
		DWORD numberOfBytesRecvd = 0;
		DWORD flags = 0;
		WSARecv(clientSocket, &wsaBuf, 1, &numberOfBytesRecvd, &flags, &overlapped, nullptr);

		DWORD numberOfBytesTransferred = 0;
		OVERLAPPED* poverlapped = nullptr;
		SOCKET completionKey = INVALID_SOCKET;
		BOOL ret = GetQueuedCompletionStatus(HIOCP, &numberOfBytesTransferred, &completionKey, &poverlapped, INFINITE);
		if (ret == FALSE || numberOfBytesTransferred == 0)
		{
			// 연결 문제 있음
			break;
		}
		else
		{
			std::cout << recvBuffer << std::endl;
		}
	}

	CloseHandle(HIOCP);
	return 0;
}
