/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 14:49:17 by smun              #+#    #+#             */
/*   Updated: 2022/04/06 14:41:06 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "std.hpp"
#include "channel.hpp"
#include "session.hpp"
#include "context.hpp"
#include "log.hpp"
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <string>
#include <sys/event.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cerrno>

Channel::Channel(int port, ISessionFactory* sessionFactory)
    : _listenPort(port)
    , _eventfd(-1)
    , _listenContext(NULL)
    , _killed(false)
    , _sessions()
    , _sessionFactory(sessionFactory)
    , _timerHandler()
{
}

Channel::~Channel()
{
    delete _listenContext;
    close(_eventfd);
    Log::Vp("Channel::~Channel", "채널 인스턴스가 삭제됩니다.");
}

void    Channel::Init()
{
    _eventfd = kqueue();
    if (_eventfd < 0)
        throw std::runtime_error("failed kqueue()");
    Log::Vp("Channel::Init", "kqueue 핸들을 받았습니다. (%d)", _eventfd);
}

void    Channel::BindAndListen()
{
    // 1. 소켓 생성
    int listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd < 0)
        throw std::runtime_error("포트 바인딩을 위한 socket() 함수 호출 실패");
    int enable = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        throw std::runtime_error("포트 바인딩을 위한 setsockopt(SO_REUSEADDR, 1) 함수 호출 실패");
    Log::Vp("Channel::BindAndListen", "연결 수락(listen) 소켓을 생성했습니다. (%d)", listenfd);

    // 2. 포트 바인딩을 위해 sockaddr 구조체 설정.
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = PF_INET;
    addr.sin_port = htons(_listenPort);
    addr.sin_addr.s_addr = INADDR_ANY;
    Log::Vp("Channel::BindAndListen", "포트 바인딩을 위해 sockaddr 구조체가 설정되었습니다. (Port %d)", _listenPort);

    // 3. 포트 바인딩
    sockaddr* paddr = reinterpret_cast<sockaddr*>(&addr);
    int bindres = bind(listenfd, paddr, sizeof(*paddr));
    if (bindres < 0)
        throw std::runtime_error("bind() 함수 호출 실패");
    Log::Dp("Channel::BindAndListen", "포트 바인딩에 성공했습니다. (Port %d)", _listenPort);

    // 4. 연결을 수락하기 시작. (backlog: 20)
    int listenres = listen(listenfd, 20);
    if (listenres < 0)
        throw std::runtime_error("listen() 함수 호출 실패");
    _listenContext = new Context(this, listenfd);
    Log::Ip("Channel::BindAndListen", "포트 %d에서 연결을 대기중입니다.", _listenPort);

    // 5. 연결을 수락하기 위해, kqueue에 read 이벤트 등록.
    SetEvent(listenfd, IOEvent_Read, IOFlag_Add | IOFlag_Enable, _listenContext);
}

void    Channel::SetEvent(int fd, int events, int flags, Context* context)
{
    struct kevent   ev[3];
    int             eventNum = 0;
    int             eventFlags = 0;

    // 내가 만든 IOFlag 플래그를 실제 kqueue에서 사용되는 플래그로 변환.
    if (flags & IOFlag_Add)
        eventFlags |= EV_ADD;
    if (flags & IOFlag_Remove)
        eventFlags |= EV_DELETE;
    if (flags & IOFlag_Enable)
        eventFlags |= EV_ENABLE;
    if (flags & IOFlag_Disable)
        eventFlags |= EV_DISABLE;
    if (flags & IOFlag_OneShot)
        eventFlags |= EV_ONESHOT;

    // kqueue64를 위해 이벤트 등록 또는 삭제 구조체를 설정.
    if (events & IOEvent_Read)
         EV_SET(&ev[eventNum++], fd, EVFILT_READ, eventFlags, 0, 0, context);
    if (events & IOEvent_Write)
         EV_SET(&ev[eventNum++], fd, EVFILT_WRITE, eventFlags, 0, 0, context);
    if (events & IOEvent_Close)
         EV_SET(&ev[eventNum++], fd, EVFILT_USER, eventFlags, NOTE_TRIGGER, 0, context);

    // 실제 kqueue에 이벤트를 등록 또는 삭제 요청.
    int evregist = kevent(_eventfd, ev, eventNum, NULL, 0, NULL);
    if (evregist < 0)
        throw std::runtime_error("kevent() 함수 호출 실패");
}

void    Channel::Accept()
{
    static int socketIdCounter = 0;
    sockaddr_in remoteaddr;
    socklen_t remoteaddr_size = sizeof(remoteaddr);

    // Accept 큐에 쌓여 있는, 클라이언트 접속 요청 하나를 accept 함수를 통해 가져옴.
    int clientfd = accept(_listenContext->GetSocket(), reinterpret_cast<sockaddr*>(&remoteaddr), &remoteaddr_size);
    if (clientfd < 0)
    {
        Log::Ep("Channel::Accept", "accept() 함수 호출 실패");
        return;
    }

    try
    {
        // 새로운 세션 인스턴스 생성.
        int socketId = ++socketIdCounter;
        const std::string addr = inet_ntoa(remoteaddr.sin_addr);
        SharedPtr<Session> session = SharedPtr<Session>(

            // ISessionFactory 인터페이스를 구현한 팩토리를 사용해서, 세션을 새로이 생성합니다.
            // Session 객체는 '세션'의 행동만을 담당하게 됩니다. (OSI 6계층)
            // 데이터 처리 등, 애플리케이션 레벨(OSI 7계층) 에서 처리되는 로직은,
            // 자식 클래스로 그 역할을 일부 이전시켜서 코드의 가독성을 높이고, Session 클래스의 과도한 책임을 덜기 위함입니다.
            _sessionFactory->CreateSession(this, clientfd, socketId, addr)

        );
        _sessions[socketId] = session;
        Log::Ip("Channel::Accept", "%s 에서 연결이 들어왔습니다.", addr.c_str());
        SetNonBlock(clientfd);
        SetEvent(clientfd, IOEvent_Read, IOFlag_Add | IOFlag_Enable, session.Load());
    }
    catch (std::exception& ex)
    {
        // 세션 인스턴스 생성 및 등록에 실패할 경우, 에러 출력.
        Log::Fp("Channel::Accept", "새로운 클라이언트 세션 인스턴스를 생성할 수 없었습니다. (%s)", ex.what());
    }
}

void    Channel::Read(Session* session)
{
    if (session == NULL) throw std::runtime_error("Channel::Read 함수로 전달된 포인터가 세션 인스턴스를 가리키고 있지 않습니다.");
    Log::Vp("Channel::Read", "[%d/%s] 읽기 이벤트가 발생되었습니다.", session->GetSocket(), session->GetRemoteAddress().c_str());
    session->OnRead();
}

void    Channel::Write(Session* session)
{
    if (session == NULL) throw std::runtime_error("Channel::Write 함수로 전달된 포인터가 세션 인스턴스를 가리키고 있지 않습니다.");
    Log::Vp("Channel::Write", "[%d/%s] 쓰기 이벤트가 발생되었습니다.", session->GetSocket(), session->GetRemoteAddress().c_str());
    session->OnWrite();
}

void    Channel::Close(Session* session)
{
    if (session == NULL) throw std::runtime_error("Channel::Close 함수로 전달된 포인터가 세션 인스턴스를 가리키고 있지 않습니다.");
    Log::Vp("Channel::Close", "[%d/%s] 세션을 채널에서 해제합니다.", session->GetSocket(), session->GetRemoteAddress().c_str());
    Log::Ip("Channel::Close", "%s 와 연결이 끊겼습니다.", session->GetRemoteAddress().c_str());
    _sessions.erase(session->GetSocketId());
}

void    Channel::Run()
{
    // 한 번에 최대로 처리할 수 있는 이벤트 개수.
    const int MaxEvents = 20;
    struct kevent events[MaxEvents];

    // 서버가 살아있을 동안 계속 반복.
    while (_killed == false)
    {
        // kqueue에서, 쌓여 있을 IO가능(ready) 이벤트를 모두 가져옴.
        int numbers = kevent(_eventfd, NULL, 0, events, MaxEvents, NULL);
        if (numbers < 0)
        {
            Log::Vp("Channel::Run", "kevent error", errno);
            continue;
        }
        Log::Vp("Channel::Run", "kevent에서 이벤트가 %d개 발생했습니다.", numbers);
        for (int i = 0; i < numbers; i++)
        {
            struct kevent& event = events[i];
            int filter = event.filter;
            Context* context = reinterpret_cast<Context*>(event.udata);

            // 처리 시작.
            try
            {
                // 연결 수락
                if (filter == EVFILT_READ && _listenContext == context)
                    Accept();

                // 데이터 읽기
                else if (filter == EVFILT_READ)
                    Read(dynamic_cast<Session*>(context));

                // 데이터 쓰기
                else if (filter == EVFILT_WRITE)
                    Write(dynamic_cast<Session*>(context));

                // 세션 닫기
                else if (filter == EVFILT_USER)
                    Close(dynamic_cast<Session*>(context));

                // 타이머 발동
                else if (filter == EVFILT_TIMER && _timerHandler != NULL)
                    _timerHandler->OnTimer();
            }
            // 예외 처리
            catch (const std::exception& ex)
            {
                Log::Fp("Channel::Run", "소켓 이벤트 처리 중 오류가 발생했습니다. [%s]", ex.what());
            }
        }
    }
}

void    Channel::SetNonBlock(int socketfd)
{
    int setfcntlres = fcntl(socketfd, F_SETFL, O_NONBLOCK);
    if (setfcntlres < 0)
        throw std::runtime_error("failed fcntl() for set nonblocking flag");
}

Session&    Channel::FindSession(int sessionKey)
{
    SessionMapIterator found = _sessions.find(sessionKey);
    if (found == _sessions.end())
        throw std::runtime_error("Not found session");
    return *(found->second.Load());
}

void    Channel::AddTimer(ITimerHandler* timerHandler)
{
    struct kevent   ev;
    if (_timerHandler != NULL) // 이전에 있있는 타이머를 삭제.
    {
        EV_SET(&ev, 1, EVFILT_TIMER, EV_DELETE, 0, 0, 0);
        if (kevent(_eventfd, &ev, 1, NULL, 0, NULL) < 0)
            throw std::runtime_error("timer1 - kevent() 함수 호출 실패");
        Log::Vp("Channel::AddTimer", "%lu초 단위의 기존 타이머가 삭제되었습니다.", _timerHandler->GetInterval());
    }
    if (timerHandler != NULL) // 새로운 타이머가 있다면 새로이 설정.
    {
        EV_SET(&ev, 1, EVFILT_TIMER, EV_ADD, NOTE_SECONDS, timerHandler->GetInterval(), 0);
        if (kevent(_eventfd, &ev, 1, NULL, 0, NULL) < 0)
            throw std::runtime_error("timer2 - kevent() 함수 호출 실패");
        Log::Vp("Channel::AddTimer", "%lu초 단위의 타이머가 추가되었습니다.", timerHandler->GetInterval());
    }
    _timerHandler = timerHandler;
}
