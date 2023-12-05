#pragma once
#include <map>
#include <mutex>

struct OverlappedEx
{

	OVERLAPPED overlapped = {};
	unsigned int type = 0; // recv = 1;
};

struct session
{
	// cilent socket
	SOCKADDR_IN clientaddr = { };
	int clientAddrLen = 0;
	SOCKET clientSocket = INVALID_SOCKET;

	// IOCP
	OverlappedEx* overlappedEx = nullptr;
	WSABUF wsaBuf = { 0 };
	DWORD numberOfBytesRecvd = 0;
	DWORD flags = 0;

	CHAR* sendBuffer = nullptr;
	
	session();
	~session();
	void clearRecvBuf();
	void clearOverlapped(unsigned int type);
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
	// 새션 가져오기
	std::shared_ptr<session> getSession(SOCKET _socket);

	// broadcast

	// unicast

private:
	std::mutex _lock;
	// SOCKET HANDLE은 프로세스에서 유일하다.
	std::map<SOCKET, std::shared_ptr<session>>  sessionList;
};

