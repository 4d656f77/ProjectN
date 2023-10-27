#include "pch.h"
#include "sessions.h"

#define MAXBUFFERSIZE 1000
session::session()
{
	overlapped = new OVERLAPPED;
	recvBuffer = new char[MAXBUFFERSIZE];
	sendBuffer = new char[MAXBUFFERSIZE];
}

session::~session()
{
	closesocket(clientSocket);
	delete overlapped;
	delete[] recvBuffer;
	delete[] sendBuffer;
	
}



sessions::sessions()
{

}

sessions::~sessions()
{
	std::lock_guard<std::mutex> lock(_lock);
	//  sseion들의 자원 할당 해제
	for (auto iter = sessionList.begin(); iter != sessionList.end(); ++iter)
	{
		// 참조 카운트 제거
		sessionList.erase(sessionList.begin(), sessionList.end());
		
	}
}

void sessions::addSession(SOCKET _socket, std::shared_ptr<session>&& _session)
{
	std::lock_guard<std::mutex> lock(_lock);
	sessionList[_socket] = _session;
}

void sessions::delSession(SOCKET _socket)
{
	std::lock_guard<std::mutex> lock(_lock);
	auto iter = sessionList.find(_socket);
	if (sessionList.end() != iter)
	{
		// 참조 카운트 제거
		sessionList.erase(iter);
	}
}


