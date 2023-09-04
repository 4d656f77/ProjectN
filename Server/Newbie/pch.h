// 프로젝트 컴파일 옵션에서 /Yu를 사용하면
// cpp파일들을 컴파일 할 때
// 미리 컴파일된 헤더가 있으면 그것을 사용한다.

#pragma once

 
// Define WIN32_LEAN_AND_MEAN to exclude APIs such as Cryptography, DDE, RPC, Shell, and Windows Sockets.
// Windows.h를 보면 몇 개의 헤더를 제외 시킨다.
#define WIN32_LEAN_AND_MEAN

// Master include file for Windows applications.
// 윈도우 어플리케이션들을 한 곳에서 모아주는 역할을 한다.
#include <Windows.h>

// definitions to be used with the WinSock 2 DLL and WinSock 2 applications.
// 윈속 api가 정의 되어 있는 헤더
#include <winsock2.h>

// This module contains the Microsoft-specific extensions to the Windows Sockets API.
// 접미사 EX가 붙은 api가 있다.
#include <mswsock.h>

// all the TCP/IP specific definitions that were included in the WINSOCK.H file are now included in WINSOCK2.H file
// 프로토콜에 대한 정보가 정의 되어 있는 헤더
#include <ws2tcpip.h>

// 컴파일 할 때 static library를 링크 시킨다.
#pragma comment(lib, "ws2_32.lib")

#include "packets.h"


#include <vector>
#include <iostream>
#include <thread>
#include <chrono>