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
	
	// 새션 추가
	void addSession(SOCKET _socket, std::shared_ptr<session>&& _session);
	// 새션 제거
	void delSession(SOCKET _socket);

	// broadcast

	// unicast

private:
	std::mutex _lock;
	// SOCKET HANDLE은 프로세스에서 유일하다.
	std::map<SOCKET, std::shared_ptr<session>>  sessionList;
};

