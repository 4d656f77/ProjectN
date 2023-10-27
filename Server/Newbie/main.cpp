#include "pch.h"
// �ӽ� �۷ι� ����
sessions SessionManager;

// ��������� ������ �Լ�
// unsigned ( __stdcall *start_address )( void * )
// unsigned __stdcall WorkerThreadFunc(LPVOID lpParam)
unsigned __stdcall  recvDispatcher(LPVOID lpParam)
{
	while (true)
	{
		DWORD numberOfBytesTransferred = 0;
		OVERLAPPED* poverlapped = nullptr;
		// ���� Ű
		SOCKET* completionKey = nullptr;
		BOOL ret = GetQueuedCompletionStatus(*((HANDLE*)lpParam), &numberOfBytesTransferred, (PULONG_PTR)&completionKey, &poverlapped, INFINITE);
		if (ret == FALSE || numberOfBytesTransferred == 0)
		{
			// ���� ���� ����
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
	// ���� �α��� ���� ĳ��
	DB database;
	std::wcout << L"database state : " << database.getState() << std::endl;
	SQLWCHAR statementText[] = L"SELECT * FROM Users";
	database.requestQuery(statementText);
	
	
	WSADATA wsaData;
	int iResult;
	// The WSAStartup function initiates use of the Winsock DLL by a process.
	// 2.2 ���� ���
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::wcout << L"WSAStartup failed : " << iResult << std::endl;
		return 1;
	}

	// ���Ͽ� ����� ������ �ʱ�ȭ
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

	// ���� ���� Ȯ��
	// ���ν������ �ݺ��ϸ鼭 ������ �޴´�.

	// IOCP ����
	IOCP iocp;


	// workerthread ����
	for (int i = 0; i < 8; ++i)
	{
		unsigned int ThreadId;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, recvDispatcher, iocp.getHendle(), 0, &ThreadId);
	}

	

	while (true)
	{
	
	// �� ���� ����
	std::shared_ptr<session> _newSession = std::make_shared<session>();
	
	// ���� ���� �ʱ�ȭ
	ZeroMemory(&_newSession->clientaddr, sizeof(_newSession->clientaddr));
	_newSession->clientAddrLen = sizeof(_newSession->clientaddr);
		

	_newSession->clientSocket = accept(listenSocket, (sockaddr*)&_newSession->clientaddr, &_newSession->clientAddrLen);
	// ���� ������ ���� �б�� �Ѿ �� newSeesion �Ҹ��� ȣ��?
	if (_newSession->clientSocket == INVALID_SOCKET)
	{
		std::wcout << L"accept failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		continue;
	}

	
	// overlapped Ȯ�� �ʿ� send���� recv���� �и�
	
	// ���̱� �˰��� ����
	// ���̱� �˰����� ����ϴ� ������ trade off�̴�.
	// ���� �����Ͱ� 1Byte�ε� 100���� ������ �ȴٸ�
	// ��Ŷ�� ����� ũ�Ⱑ ��ũ�Ƿ� ��ȿ�����̴�.
	// ��Ƽ� ������ �ȴٸ� ������ �帧�� ȿ�����ε� ������(���� ������ �ʴ´�.)�� �������� �ȴ�.
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

	// IOCP�� Ŭ���̾�Ʈ ���� ���
	
	if (iocp.AssociateDeviceWithCompletionPort(*iocp.getHendle(), (HANDLE)_newSession->clientSocket, (ULONG_PTR)&_newSession->clientSocket) == false)
	{
		std::wcout << L"AssociateDeviceWithCompletionPort failed with error" << std::endl;
		closesocket(listenSocket);
		closesocket(_newSession->clientSocket);
		WSACleanup();
		return 1;
	}

	
	// send socket number << ���ϳѹ� ������
	//	// OVERLAPPED ����ü �ʱ�ȭ
	//	OVERLAPPED overlapped;
	//	ZeroMemory(&overlapped, sizeof(overlapped));

	//	// ���� ���� �ʱ�ȭ, ������ + ����
	//	WCHAR sendBuffer[] = L"test";
	//	WSABUF wsaBuf;
	//	wsaBuf.buf = (CHAR*)sendBuffer;
	//	wsaBuf.len = sizeof(sendBuffer);

	//	DWORD numberOfBytesSent = 0;
	//	// flag 0�� �ǹ��ϴ� ����??
	//	DWORD flag = 0;
	//	WSASend(clientSocket, &wsaBuf, 1, &numberOfBytesSent, flag, &overlapped, nullptr);
	//	//DWORD numberOfBytesTransferred = 0;
	//	//OVERLAPPED* poverlapped = nullptr;
	//	//ULONG_PTR completionKey = NULL;
	//	//// ������ ���� ��� ���� ���
	//	//BOOL ret = GetQueuedCompletionStatus(HIOCP, &numberOfBytesTransferred, &completionKey, &poverlapped, INFINITE);
	//	//// ������ ���۵� ����Ʈ�� �ƴ϶� send request�� ���� ����Ʈ��.
	//	//{
	//	//	std::wcout << L"���� ����Ʈ : " << numberOfBytesTransferred << std::endl;
	//	//}


	// recv�� recvDispatcher�� ������ �ش�
	
	//// OVERLLAPED ����ü Ŭ���� RECV => 1
	_newSession->clearOverlapped(1);
	
	//// Recv�� �ޱ����� ���۱��� + ���������� Ŭ����
	_newSession->clearRecvBuf();

	//// dwBufferCount : The number of WSABUF structures
	WSARecv(_newSession->clientSocket, &_newSession->wsaBuf, 1, &_newSession->numberOfBytesRecvd, &_newSession->flags, (OVERLAPPED*)&_newSession->overlapped->overlapped, nullptr);
	//// ^^^ I/O request

	// ���� �߰�
	SessionManager.addSession(_newSession->clientSocket, std::move(_newSession));

	}

	return 0;
}