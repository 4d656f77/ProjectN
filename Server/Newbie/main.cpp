#include "pch.h"
// 임시 글로벌 변수
CHAR* recvBuffer;
OVERLAPPED* overlapped;

HANDLE CreateNewIoCompletionPort(DWORD dwNumberOfConcurrentThreads)
{
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
}

BOOL AssociateDeviceWithCompletionPort(HANDLE hExistingCompletionPort, HANDLE hFileHandle, ULONG_PTR ulpCompletionKey)
{
	HANDLE _HIOCP = CreateIoCompletionPort(hFileHandle, hExistingCompletionPort, ulpCompletionKey, 0);

	return (_HIOCP == hExistingCompletionPort);
}

// 스레드들이 실행할 함수
// unsigned ( __stdcall *start_address )( void * )
// unsigned __stdcall WorkerThreadFunc(LPVOID lpParam)
unsigned __stdcall  recvDispatcher(LPVOID lpParam)
{
	while (true)
	{
		DWORD numberOfBytesTransferred = 0;
		OVERLAPPED* poverlapped = nullptr;
		SOCKET* completionKey = nullptr;
		BOOL ret = GetQueuedCompletionStatus(*((HANDLE*)lpParam), &numberOfBytesTransferred, (PULONG_PTR)&completionKey, &poverlapped, INFINITE);
		if (ret == FALSE || numberOfBytesTransferred == 0)
		{
			// 연결 문제 있음
			break;
		}
		else
		{

			struct packets::characterPhysInfo* packetoffset = reinterpret_cast<struct packets::characterPhysInfo*>(recvBuffer);
			std::wcout << L"Location" << " : " <<
				packetoffset->Location.X << ", " <<
				packetoffset->Location.Y << ", " <<
				packetoffset->Location.Z << std::endl;
			std::wcout << L"Rotation" << " : " <<
				packetoffset->Rotation.Pitch << ", " <<
				packetoffset->Rotation.Yaw << ", " <<
				packetoffset->Rotation.Roll << std::endl;
			std::wcout << L"Velocity" << " : " <<
				packetoffset->Velocity.X << ", " <<
				packetoffset->Velocity.Y << ", " <<
				packetoffset->Velocity.Z << std::endl;
		}

		WSABUF wsaBuf;
		wsaBuf.buf = recvBuffer;
		wsaBuf.len = 100;
		DWORD numberOfBytesRecvd = 0;
		DWORD flags = 0;

		ZeroMemory(poverlapped, sizeof(OVERLAPPED));

		WSARecv(*completionKey, &wsaBuf, 1, &numberOfBytesRecvd, &flags, poverlapped, nullptr);
	}
	return 0;
}


int main()
{
	DB database;
	std::wcout << L"database state : " << database.getState() << std::endl;
	SQLWCHAR statementText[] = L"SELECT * FROM Users";
	database.requestQuery(statementText);
	WSADATA wsaData;


	int iResult;
	// The WSAStartup function initiates use of the Winsock DLL by a process.
	// 2.2 버전 사용
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::wcout << L"WSAStartup failed : " << iResult << std::endl;
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
		std::wcout << L"Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	iResult = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (iResult == SOCKET_ERROR)
	{
		std::wcout << L"bind failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	
	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		std::wcout << L"listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// 유저 접속 확인


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
		std::wcout << L"accept failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1; 
	}
	// 네이글 알고리즘 끄기
	// 네이글 알고리즘을 사용하는 기준은 trade off이다.
	// 보낼 데이터가 1Byte인데 100번을 보내게 된다면
	// 패킷의 헤더의 크기가 더크므로 비효율적이다.
	// 모아서 보내게 된다면 데이터 흐름은 효율적인데 반응성(자주 보내지 않는다.)은 떨어지게 된다.
	BOOL bOptVal = FALSE;
	int bOptLen = sizeof(BOOL);
	iResult = setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&bOptVal, bOptLen);
	if (iResult == SOCKET_ERROR)
	{
		std::wcout << L"setsockopt for TCP_NODELAY failed with error : " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// IOCP에 클라이언트 소켓 등록
	
	if (AssociateDeviceWithCompletionPort(HIOCP, (HANDLE)clientSocket, (ULONG_PTR)&clientSocket) == false)
	{
		std::wcout << L"AssociateDeviceWithCompletionPort failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	

		// recv 등록
		// IOCP에 등록한 소켓의 api 결과를 확인하기 위해서 OVERLLAPED 구조체가 필요하다.
		// 해제 대신 재사용
		// 따로 메모리 관리 할 필요가 있음
		overlapped = new OVERLAPPED;
		ZeroMemory(overlapped, sizeof(OVERLAPPED));


		// Recv를 받기위한 버퍼길이 + 버퍼포인터
		// 해제 대신 재사용
		// 따로 메모리 관리 할 필요가 있음
		recvBuffer = new char[100];
		WSABUF wsaBuf;
		wsaBuf.buf = recvBuffer;
		wsaBuf.len = 100;


		// dwBufferCount : The number of WSABUF structures
		DWORD numberOfBytesRecvd = 0;
		DWORD flags = 0;
		WSARecv(clientSocket, &wsaBuf, 1, &numberOfBytesRecvd, &flags, overlapped, nullptr);
		// ^^^ I/O request

		// workerthread 생성
		for (int i = 0; i < 8; ++i)
		{
			unsigned int ThreadId;
			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, recvDispatcher, &HIOCP, 0, &ThreadId);
		}
		//recvDispatcher(HIOCP);
		while (true)
		{ 
		}

	// send
	//while (true)
	//{
	//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	//	// OVERLAPPED 구조체 초기화
	//	OVERLAPPED overlapped;
	//	ZeroMemory(&overlapped, sizeof(overlapped));


	//	// 보낼 버퍼 초기화, 포인터 + 길이
	//	WCHAR sendBuffer[] = L"test";
	//	WSABUF wsaBuf;
	//	wsaBuf.buf = (CHAR*)sendBuffer;
	//	wsaBuf.len = sizeof(sendBuffer);

	//	DWORD numberOfBytesSent = 0;
	//	// flag 0이 의미하는 것은??
	//	DWORD flag = 0;
	//	WSASend(clientSocket, &wsaBuf, 1, &numberOfBytesSent, flag, &overlapped, nullptr);
	//	//DWORD numberOfBytesTransferred = 0;
	//	//OVERLAPPED* poverlapped = nullptr;
	//	//ULONG_PTR completionKey = NULL;
	//	//// 연결이 끊긴 경우 무한 대기
	//	//BOOL ret = GetQueuedCompletionStatus(HIOCP, &numberOfBytesTransferred, &completionKey, &poverlapped, INFINITE);
	//	//// 실제로 전송된 바이트가 아니라 send request에 대한 바이트다.
	//	//{
	//	//	std::wcout << L"전송 바이트 : " << numberOfBytesTransferred << std::endl;
	//	//}
	//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	//}
	CloseHandle(HIOCP);
	return 0;
}