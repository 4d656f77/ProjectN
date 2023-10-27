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
	//  sseion���� �ڿ� �Ҵ� ����
	for (auto iter = sessionList.begin(); iter != sessionList.end(); ++iter)
	{
		// ���� ī��Ʈ ����
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
		// ���� ī��Ʈ ����
		sessionList.erase(iter);
	}
}


