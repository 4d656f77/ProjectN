#include "pch.h"
// 임시 글로벌 변수
sessions SessionManager;

// 스레드들이 실행할 함수
// unsigned ( __stdcall *start_address )( void * )
// unsigned __stdcall WorkerThreadFunc(LPVOID lpParam)
unsigned __stdcall  recvDispatcher(LPVOID lpParam)
{
	while (true)
	{
		DWORD numberOfBytesTransferred = 0;
		OVERLAPPED* poverlapped = nullptr;
		// 세션 키
		SOCKET* completionKey = nullptr;
		BOOL ret = GetQueuedCompletionStatus(*((HANDLE*)lpParam), &numberOfBytesTransferred, (PULONG_PTR)&completionKey, &poverlapped, INFINITE);
		if (ret == FALSE || numberOfBytesTransferred == 0)
		{
			// 연결 문제 있음
			break;
		}
		else
		{
		
			std::shared_ptr<session> curSession = SessionManager.getSession(*completionKey);


			struct packets::characterPhysInfo* packetoffset = reinterpret_cast<struct packets::characterPhysInfo*>(curSession->wsaBuf.buf);
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
			
			
			
			curSession->clearOverlapped(1);
			curSession->clearRecvBuf();

			WSARecv(*completionKey, &curSession->wsaBuf, 1, &curSession->numberOfBytesRecvd, &curSession->flags, (OVERLAPPED*)&curSession->overlapped->overlapped, nullptr);

		}
	}
	return 0;
}


int main()
{
	// 유저 로그인 정보 캐싱
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
	// 메인스레드는 반복하면서 유저를 받는다.

	// IOCP 생성
	IOCP iocp;


	// workerthread 생성
	for (int i = 0; i < 8; ++i)
	{
		unsigned int ThreadId;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, recvDispatcher, iocp.getHendle(), 0, &ThreadId);
	}

	

	while (true)
	{
	
	// 빈 새션 생성
	std::shared_ptr<session> _newSession = std::make_shared<session>();
	
	// 소켓 정보 초기화
	ZeroMemory(&_newSession->clientaddr, sizeof(_newSession->clientaddr));
	_newSession->clientAddrLen = sizeof(_newSession->clientaddr);
		

	_newSession->clientSocket = accept(listenSocket, (sockaddr*)&_newSession->clientaddr, &_newSession->clientAddrLen);
	// 소켓 에러로 다음 분기로 넘어갈 때 newSeesion 소멸자 호출?
	if (_newSession->clientSocket == INVALID_SOCKET)
	{
		std::wcout << L"accept failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		continue;
	}

	
	// overlapped 확장 필요 send인지 recv인지 분리
	
	// 네이글 알고리즘 끄기
	// 네이글 알고리즘을 사용하는 기준은 trade off이다.
	// 보낼 데이터가 1Byte인데 100번을 보내게 된다면
	// 패킷의 헤더의 크기가 더크므로 비효율적이다.
	// 모아서 보내게 된다면 데이터 흐름은 효율적인데 반응성(자주 보내지 않는다.)은 떨어지게 된다.
	//BOOL bOptVal = FALSE;
	//int bOptLen = sizeof(BOOL);
	//iResult = setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&bOptVal, bOptLen);
	//if (iResult == SOCKET_ERROR)
	//{
	//	std::wcout << L"setsockopt for TCP_NODELAY failed with error : " << WSAGetLastError() << std::endl;
	//	closesocket(clientSocket);
	//	closesocket(listenSocket);
	//	WSACleanup();
	//	return 1;
	//}

	// IOCP에 클라이언트 소켓 등록
	
	if (iocp.AssociateDeviceWithCompletionPort(*iocp.getHendle(), (HANDLE)_newSession->clientSocket, (ULONG_PTR)&_newSession->clientSocket) == false)
	{
		std::wcout << L"AssociateDeviceWithCompletionPort failed with error" << std::endl;
		closesocket(listenSocket);
		closesocket(_newSession->clientSocket);
		WSACleanup();
		return 1;
	}

	
	// send socket number << 소켓넘버 보내기
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


	// recv는 recvDispatcher가 실행해 준다
	
	//// OVERLLAPED 구조체 클리어 RECV => 1
	_newSession->clearOverlapped(1);
	
	//// Recv를 받기위한 버퍼길이 + 버퍼포인터 클리어
	_newSession->clearRecvBuf();

	//// dwBufferCount : The number of WSABUF structures
	WSARecv(_newSession->clientSocket, &_newSession->wsaBuf, 1, &_newSession->numberOfBytesRecvd, &_newSession->flags, (OVERLAPPED*)&_newSession->overlapped->overlapped, nullptr);
	//// ^^^ I/O request

	// 세션 추가
	SessionManager.addSession(_newSession->clientSocket, std::move(_newSession));

	}

	return 0;
}