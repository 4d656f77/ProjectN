> # Project N
> 
> - to study a multi-play game server

> # Index
> 
> - [Precompiled Headers](#precompiled-headers)
> - [I/O Completion Ports](#i/o-completion-ports)

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


