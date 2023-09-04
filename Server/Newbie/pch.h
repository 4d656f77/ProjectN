// ������Ʈ ������ �ɼǿ��� /Yu�� ����ϸ�
// cpp���ϵ��� ������ �� ��
// �̸� �����ϵ� ����� ������ �װ��� ����Ѵ�.

#pragma once

 
// Define WIN32_LEAN_AND_MEAN to exclude APIs such as Cryptography, DDE, RPC, Shell, and Windows Sockets.
// Windows.h�� ���� �� ���� ����� ���� ��Ų��.
#define WIN32_LEAN_AND_MEAN

// Master include file for Windows applications.
// ������ ���ø����̼ǵ��� �� ������ ����ִ� ������ �Ѵ�.
#include <Windows.h>

// definitions to be used with the WinSock 2 DLL and WinSock 2 applications.
// ���� api�� ���� �Ǿ� �ִ� ���
#include <winsock2.h>

// This module contains the Microsoft-specific extensions to the Windows Sockets API.
// ���̻� EX�� ���� api�� �ִ�.
#include <mswsock.h>

// all the TCP/IP specific definitions that were included in the WINSOCK.H file are now included in WINSOCK2.H file
// �������ݿ� ���� ������ ���� �Ǿ� �ִ� ���
#include <ws2tcpip.h>

// ������ �� �� static library�� ��ũ ��Ų��.
#pragma comment(lib, "ws2_32.lib")

#include "packets.h"


#include <vector>
#include <iostream>
#include <thread>
#include <chrono>