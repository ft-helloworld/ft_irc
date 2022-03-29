/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 14:43:33 by smun              #+#    #+#             */
/*   Updated: 2022/03/29 17:57:21 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include "shared_ptr.hpp"

class Context;
class Session;

/*
**  Channel 클래스.
**
**  네트워크 입출력과 관련된 모든 작업을 관리합니다.
*/

class Channel
{
private:
    typedef     std::map<int, SharedPtr<Session> >             SessionMap;
    typedef     std::map<int, SharedPtr<Session> >::iterator   SessionMapIterator;

    /**
     * 연결을 대기할 포트 번호입니다.
     */
    const int   _listenPort;

    /**
     * kqueue를 가리키는 fd 입니다.
     */
    int         _eventfd;

    /**
     * 연결을 대기하고 있는 listen 소켓을 가지고 있는 Context 인스턴스 입니다.
     *
     * kqueue의 이벤트는 udata 라고 부르는 포인터를 가질 수 있습니다. 이 udata는 'U'ser 'Data'의 줄임말이며,
     * 각 이벤트는 사용자가 미리 정의한 포인터를 udata 변수에 저장해서 전달할 수 있는 구조를 갖고 있습니다.
     *
     * Channel 클래스는, 해당 udata를 Context 클래스의 포인터로 관리합니다.
     * 그리고, 연결을 수락하는 accept는 일종의 'read' 이벤트로 처리됩니다.
     * 따라서 accept 이벤트 역시도 다른 read 및 write 이벤트와 하나의 로직으로 묶기 위해서는, 연결을 수락할 listen 소켓 역시도
     * 일반적인 Context 클래스로 관리될 필요가 있습니다.
     */
    Context*    _listenContext;

    /**
     * 시그널 등으로 서버가 중단이 요청될 경우, 서버의 이벤트 루프를 중단시키기 위한 스위치 입니다.
     * 그렇게 구현하려면 EVFILT_USER 이벤트가 필요하고, 그러한 처리가 추가적으로 필요하나, 과제의 필수 여부는 아직 확실하지 않습니다.
     */
    bool        _killed;

    /**
     * 현재 채널에 연결된 세션들을 저장하는 맵입니다.
     * 채널에 연결이 들어오면, 채널은 새로운 세션 인스턴스를 생성합니다.
     * 키는 세션마다 고유하게 갖는 ID이며, 값에는 생성된 새로운 세션 인스턴스가 공유 포인터로 들어갑니다.
     *
     * 세션 인스턴스는 그 특성 상 복사되어서는 안됩니다. 복사될 경우, 같은 소켓 fd를 갖는 인스턴스가 2개 이상이 되어
     * 소켓이 올바르게 동작하지 않을 가능성이 생기기 때문입니다. 따라서 Session 클래스는 복사 생성자 및 대입 연산자를 허용하지 않고 있습니다.
     * 하지만 std::map 에 값을 넣기 위해서는 대입 연산자 또는 복사 생성자가 필요하게 되는 모순을 갖게 됩니다.
     * 따라서 이러한 모순을 해결하기 위해, 공유 포인터를 사용하여 Session 클래스가 필요할 때 올바르게 소멸자가 호출되며 삭제될 수 있도록 제어합니다.
     *
     * 세션 인스턴스의 소멸자가 중요한 이유는, 해당 소멸자에서 소켓의 close 메서드 호출 및 kqueue 이벤트 등록 해제가 이루어지기 때문입니다.
     */
    SessionMap  _sessions;

    Channel();
    Channel(const Channel&);
    Channel&    operator=(const Channel&);

    /**
     * @brief 연결을 수락할 때 사용하는 함수입니다.
     * @details 이 함수를 실행하여서 backlog에 있는 socket을 accept를 사용해 꺼내오고,
     * 새로운 세션 인스턴스를 만든 후 세션 맵에 넣고, 해당 소켓에 대해 통신을 할 수 있게 IO이벤트를 kqueue에 등록합니다.
     *
     * Accept에 실패해도 예외가 발생하지는 않으며, 단순히 accept에 실패했다는 오류 메시지만 나옵니다.
     * 따라서 오류가 발생한 세션을 간단히 종료시키고, 계속해서 다른 연결을 받을 수 있습니다.
     */
    void    Accept();

    /**
     * @brief 세션에서 데이터를 읽어올 때 사용하는 함수입니다.
     * @details kevent64 함수에서 데이터를 읽을 수 있음이 확인되면,
     * 이 함수를 호출해서 시스템 커널에 저장된 데이터를 읽어 세션의 _recvBuffer에 저장합니다.
     * 이후, <CR><LF>를 구분자로 하여 내용을 구분하고, 분석하여 처리할 수 있게 합니다.
     * 처리된 데이터는 한 줄씩 Session::Process 함수를 호출하여 넘깁니다.
     *
     * 내부의 recv 함수 호출에 실패하여 문제가 발생하면, Close 함수가 호출되어 연결을 종료하게 됩니다.
     *
     * @param session 데이터를 읽을 Session 포인터 입니다.
     *
     * @exception session이 NULL이라면 std::runtime_error 예외가 발생합니다.
     */
    void    Read(Session* session);

    /**
     * @brief 세션에서 원격으로 데이터를 보낼 때 사용하는 함수입니다.
     * @details kevent64 함수에서 데이터를 1 바이트라도 보낼 수 있음이 확인되면, 이 함수를 호출해서
     * 세션의 _sendBuffer에서 데이터를 꺼내 시스템 커널에 원격으로 데이터 전송을 요청합니다.
     *
     * 보내진 데이터는 _sendBuffer에서 삭제되어 꺼내지며(Polling), 더 이상 보낼 데이터가 없다면
     * 쓰기 이벤트를 비활성화 합니다. 추후 쓰기 이벤트가 다시 활성화 되면, 이 함수가 다시 호출될 수 있습니다.
     *
     * 내부의 send 함수 호출에 실패하여 문제가 발생하면, Close 함수가 호출되어 연결을 종료하게 됩니다.
     *
     * @param session 데이터를 작성할 Session 포인터 입니다.
     *
     * @exception session이 NULL이라면 std::runtime_error 예외가 발생합니다.
     */
    void    Write(Session* session);

public:
    /**
     * @brief 채널의 생성자입니다.
     *
     * @param port 연결을 수락할 포트 번호입니다.
     */
    Channel(int port);

    virtual ~Channel();

    /**
     * @brief kqueue 함수를 호출하며 채널을 시작할 준비를 합니다.
     *
     * @exception kqueue() 함수 호출이 실패하면 std::runtime_error 예외가 발생합니다.
    */
    void    Init();

    /**
     * @brief 해당 포트에서 연결을 수락하기 위한 준비를 합니다.
     *
     * @details (socket -> bind -> setsockopt -> listen -> kevent64(SetEvent))
     *
     * @exception 실행 중 오류가 발생할 경우 std::runtime_error 예외가 발생합니다.
     */
    void    BindAndListen();

    /**
     * @brief 채널에서 해당 세션을 닫는 처리를 합니다.
     *
     * @details 여기에서 세션 맵에 있는 세션의 공유포인터가 std::map의 erase 메서드로 삭제됩니다.
     * 이후, 공유 포인터에 저장된 세션의 실제 포인터가 참조 카운트에 따라 자동으로 삭제될 것입니다.
     *
     * 참조 카운트에 따라 자동으로 삭제되면 해당 소멸자에서 소켓 fd의 close 및 kqueue에서 이벤트 등록해제 등이 모두 이루어집니다.
     *
     * @param session 세션을 닫는 처리를 할 Session 포인터 입니다.
     *
     * @exception 이벤트 해제가 일부라도 실패할 경우 std::runtime_error 예외가 발생합니다.
     */
    void    Close(Session* session);

    /**
     * 소켓 이벤트를 처리할 메인 메서드입니다.
     * _killed 변수가 거짓일 동안 계속 반복합니다.
     * kevent64 함수를 호출하여, 이벤트가 생길 때 까지 대기합니다.
     * 이벤트가 발생되면, 해당 이벤트 구조체의 udata 변수에서 Context 객체의 포인터를 얻어옵니다.
     *
     * Context 객체와 _listenContext가 같고, Read 이벤트라면 해당 이벤트는 연결을 수락하는 이벤트입니다. 따라서 Accept를 호출합니다.
     * 다른 경우에는, Read, Write 함수를 Session* 으로 dynamic_cast 하여 호출합니다.
     * dynamic_cast가 실패하여 Read, Write 매개변수로 NULL이 전달될 경우에는 예외를 발생시킵니다.
     *
     * 이벤트 처리 중 예외가 발생하면, try ~ catch 블록으로 묶여 있기 때문에 서버가 죽지는 않고,
     * 예외 메시지만 콘솔에 출력하게 됩니다.
     */
    void    Run();

    /**
     * @brief kqueue에 특정 이벤트를 등록하거나, 해제합니다.
     *
     * @param fd kqueue에서 이벤트를 등록하거나 해제하게 될 소켓 fd입니다.
     * @param events 읽기 또는 쓰기. 어떤 이벤트를 등록이나 해제할 지 결정합니다.
     * @param flags 등록 또는 삭제 등 옵션의 플래그입니다.
     * @param context 이벤트의 udata 변수로 전달됩니다.
     *
     * @exception 등록 도는 해제가 일부라도 실패할 경우 std::runtime_error 예외가 발생합니다.
     */
    void    SetEvent(int fd, int events, int flags, Context* context);

    /**
     * @brief 특정 세션 ID로 세션을 찾습니다.
     *
     * @param sessionKey 세션 ID
     * @return 세션을 발견하면 세션 ID가 발견되며, 찾지 못했다면 std::runtime_error 예외가 발생합니다.
     * @exception 세션을 발견하지 못하면 std::runtime_error 예외가 발생합니다.
     */
    Session&    FindSession(int sessionKey);
    Session&    FindSession(const std::string& user);

    /**
     * @brief 소켓을 논블로킹 소켓으로 설정합니다. (과제 참조!)
     *
     * @param socketfd 논블로킹 소켓으로 만들 소켓 fd입니다.
     */
    static void SetNonBlock(int socketfd);
};

#endif
