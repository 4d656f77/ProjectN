#include "pch.h"

// �ӽ� �۷ι� ����
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

// ��������� ������ �Լ�
void recvDispatcher(HANDLE HIOCP)
{
	while (true)
	{
		DWORD numberOfBytesTransferred = 0;
		OVERLAPPED* poverlapped = nullptr;
		SOCKET* completionKey = nullptr;
		BOOL ret = GetQueuedCompletionStatus(HIOCP, &numberOfBytesTransferred, (PULONG_PTR)&completionKey, &poverlapped, INFINITE);
		if (ret == FALSE || numberOfBytesTransferred == 0)
		{
			// ���� ���� ����
			break;
		}
		else
		{

			struct packets::characterPhysInfo* packetoffset = reinterpret_cast<struct packets::characterPhysInfo*>(recvBuffer);
			std::cout << "Location" << " : " <<
				packetoffset->Location.X << ", " <<
				packetoffset->Location.Y << ", " <<
				packetoffset->Location.Z << std::endl;
			std::cout << "Rotation" << " : " <<
				packetoffset->Rotation.Pitch << ", " <<
				packetoffset->Rotation.Yaw << ", " <<
				packetoffset->Rotation.Roll << std::endl;
			std::cout << "Velocity" << " : " <<
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
}


int main()
{
	WSADATA wsaData;


	int iResult;
	// The WSAStartup function initiates use of the Winsock DLL by a process.
	// 2.2 ���� ���
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cout << "WSAStartup failed : " << iResult << std::endl;
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

	// ���� ���� Ȯ��


	// IOCP ����
	HANDLE HIOCP = CreateNewIoCompletionPort(0);

	// Ŭ���̾�Ʈ ����
	SOCKADDR_IN clientAddr;
	ZeroMemory(&clientAddr, sizeof(clientAddr));
	int clientAddrLen = sizeof(clientAddr);

	//							����ü ������
	//		SOCKADDR_IN								SOCKADDR
	//     USHORT sin_port;  2BYTE	 			CHAR sa_data[14];
	//     IN_ADDR sin_addr; 4BYTE					
	//     CHAR sin_zero[8]; 8BYTE					

	SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "accept failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// IOCP�� Ŭ���̾�Ʈ ���� ���
	
	if (AssociateDeviceWithCompletionPort(HIOCP, (HANDLE)clientSocket, (ULONG_PTR)&clientSocket) == false)
	{
		std::cout << "AssociateDeviceWithCompletionPort failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	

		// recv ���
		// IOCP�� ����� ������ api ����� Ȯ���ϱ� ���ؼ� OVERLLAPED ����ü�� �ʿ��ϴ�.
		// ���� ��� ����
		// ���� �޸� ���� �� �ʿ䰡 ����
		overlapped = new OVERLAPPED;
		ZeroMemory(overlapped, sizeof(OVERLAPPED));


		// Recv�� �ޱ����� ���۱��� + ����������
		// ���� ��� ����
		// ���� �޸� ���� �� �ʿ䰡 ����
		recvBuffer = new char[100];
		WSABUF wsaBuf;
		wsaBuf.buf = recvBuffer;
		wsaBuf.len = 100;


		// dwBufferCount : The number of WSABUF structures
		DWORD numberOfBytesRecvd = 0;
		DWORD flags = 0;
		WSARecv(clientSocket, &wsaBuf, 1, &numberOfBytesRecvd, &flags, overlapped, nullptr);
		// ^^^ I/O request


		recvDispatcher(HIOCP);
		while (true)
		{ 
		}

	// send
	//while (true)
	//{
	//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
	//	//	std::cout << "���� ����Ʈ : " << numberOfBytesTransferred << std::endl;
	//	//}
	//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	//}
	CloseHandle(HIOCP);
	return 0;
}