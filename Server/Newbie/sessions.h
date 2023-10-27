#pragma once
#include <map>
#include <mutex>

struct OverlappedEx
{
	OVERLAPPED overlapped = {};
	int type = 0; // read, write, accept, connect ...
};

struct session
{
	// cilent socket
	SOCKADDR_IN clientaddr = { };
	int clientAddrLen = 0;
	SOCKET clientSocket = INVALID_SOCKET;

	// IOCP
	OVERLAPPED* overlapped = nullptr;
	CHAR* recvBuffer = nullptr;
	CHAR* sendBuffer = nullptr;
	
	session();
	~session();
};

class sessions
{

public:
	sessions();

	~sessions();
	
	// ���� �߰�
	void addSession(SOCKET _socket, std::shared_ptr<session>&& _session);
	// ���� ����
	void delSession(SOCKET _socket);

	// broadcast

	// unicast

private:
	std::mutex _lock;
	// SOCKET HANDLE�� ���μ������� �����ϴ�.
	std::map<SOCKET, std::shared_ptr<session>>  sessionList;
};

