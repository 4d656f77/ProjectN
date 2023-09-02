> # Project N
> 
> - to study a multi-play game server

> # Index
> 
> - [Precompiled Headers](#precompiled-headers)
> - [I/O Completion Ports](#i/o-completion-ports)
> - [Packet Serialization](#Packet-Serialization)

# Precompiled Headers

> ![](./images/precompiled%20headers_pchf.png)
> 
> 설정을 잘 완료 했으면 프로젝트 빌드 부산물 중에서 미리컴파일된헤더 파일이 생긴다.

# I/O Completion Ports

> [Microsoft Docs]([I/O Completion Ports - Win32 apps | Microsoft Learn](https://learn.microsoft.com/en-us/windows/win32/fileio/i-o-completion-ports))
> 
> 1. I/O Completion Ports가 무엇인가?
> 
> > 여러 개의 비동기적인 요청을 효율적으로 처리하는 모델이다.
> > 
> > 구체적으로 미리 할당 된 스레드 풀을 가지고 있고, 큐를 통해서 작업을 받아서 처리한다.
> 
> 2. IOCP는 어떻게 사용할까?
> 
> > `CreateIoCompletionPort` 함수를 이용해서 IOCP를 만들면 핸들값을 얻을 수 있다. 그리고 소켓을 만들어 등록하고, `CompletionKey`를 통해서 그 소켓을 식별할 수 있다.
> > 
> > 그 소켓에 대한 비동기적인 요청을 실행하기 위해서 `OVERLAPPED`구조체를 넣어준다. I/O에 대한 정보가 `OVERLAPPED`구조체에 담긴다. 
> > 
> > 마지막으로 `GetQueuedCompletionStatus`를 통해서 완료된 요청을 받아서 원하는 작업을 하면 된다.

# Packet Serialization

> 클라이언트에서 패킷을 보내면 그 패킷을 활용해서 서버 서비스를 구현한다.
> 
> 1. 구조체를 활용한 직렬화
> 
> > `#pragma pack(push, 1)`, `#pragma pack(pop)`을 이용해서 alignment를 1BYTE로 한다.
> > 
> > 받은 버퍼에 해당하는 구조체 포인터로 읽으면 클라이언트에 보낸 패킷을 그대로 활용할 수 있다.
> > 
> > 단순한 만큼 직렬화에 대한 클럭 소모가 없다.
> > 
> > 클라이언트를 UE4를 이용해서 만들었기 때문에 간단하게 패킷 직렬화를 했지만, 다른 자료형을 갖는 플렛폼과 언어를 사용할 시 이와 같은 방법은 사용하기에 적합하지 않다.


