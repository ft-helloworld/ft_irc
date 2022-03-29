/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:26:59 by smun              #+#    #+#             */
/*   Updated: 2022/03/29 17:45:00 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
#define SESSION_HPP

#include "std.hpp"
#include "context.hpp"
#include <vector>
#include <string>

class Session : public Context
{
private:
    typedef std::vector<Byte>             ByteBuffer;
    typedef std::vector<Byte>::iterator   ByteBufferIterator;

    /**
     * @brief 세션의 고유 ID 입니다. Context에 할당될 소켓 fd와는 별개의 값입니다.
     * @details Channel 내부에서 해당 세션을 관리하기 위한 값입니다.
     *
     */
    const int           _socketId;

    /**
     * @brief 원격 주소입니다. 해당 세션의 IP주소가 저장됩니다.
     *
     */
    const std::string   _remoteAddress;

    /**
     * @brief recv 함수로 읽어온 데이터를 쌓아두는 임시 버퍼입니다.
     * @details 개행(cr/lf)이 발견되면, 개행 단위로 분리하여 Process함수에 해당 줄을 넘기고,
     * 읽어온 데이터는 _recvBuffer 버퍼에서 삭제시킵니다.
     *
     */
    ByteBuffer          _recvBuffer;

    /**
     * @brief send 함수를 통해 데이터를 보내기 위해 데이터를 쌓아두는 임시 버퍼입니다.
     * @details 쓰기 이벤트가 발생되면 send함수를 사용해 이 버퍼에서 데이터를 꺼내 송신합니다.
     * send 함수를 통해 보내진 데이터 만큼 _sendBuffer 버퍼에서 삭제되어, 이후 데이터부터 보낼 수 있도록 합니다.
     *
     */
    ByteBuffer          _sendBuffer;

    /**
     * @brief kqueue에 요청된 이벤트의 플래그입니다. IOEvent_Read 또는 IOEvent_Write 가 등록될 수 있습니다.
     *
     */
    int                 _triggeredEvents;

    Session();
    Session(const Session&);
    Session& operator= (const Session&);

    /**
     * @brief _recvBuffer 버퍼에 데이터를 추가하는 함수입니다.
     *
     * @param buffer 추가할 데이터 포인터
     * @param bytes 추가할 데이터의 크기
     */
    void    AppendBuffer(Byte* buffer, size_t bytes);

    /**
     * @brief _sendBuffer 버퍼에서 데이터를 삭제할 함수입니다.
     * @details std::memmove(&_sendBuffer[0], &_sendBuffer[bytes], bytes) 와 비슷합니다.
     *
     * @param bytes _sendBuffer를 앞으로 밀어 당길 크기.
     */
    void    TakeBuffer(size_t bytes);

    /**
     * @brief 세션으로 문자열을 전송합니다. 문자열에는 여러 개행이 포함될 수 있으며, 클라이언트에서 이 개행을 각각 모두 분리하여 처리할 수도 있습니다.
     *
     * @param buf 세션으로 전송될 데이터의 포인터
     * @param len 세션으로 전송될 데이터의 크기
     */
    void    Send(const void* buf, size_t len);

    /**
     * @brief 버퍼에서 개행을 통해 줄을 분리해옵니다. 개행이 없다면, false가 반환됩니다. 개행이 있다면, true가 반환되며 line 레퍼런스에 읽어온 한 줄이 들어갑니다.
     *
     * @param buffer 개행을 찾을 버퍼 (_recvBuffer)
     * @param line 개행을 발견한다면, 한 줄을 읽어내어 저장할 string
     * @return 개행을 발견했다면 line에 한 줄이 저장되고, 읽어온 데이터 만큼 버퍼에서 삭제합니다. 그리고 true가 반환됩니다. 개행이 없었다면 아무 것도 하지 않으며, false만 반환합니다.
     */
    static bool    GetNextLine(ByteBuffer& buffer, std::string& line);

protected:
    /**
     * @brief crlf 개행으로 구분되어진 한 줄을 처리합니다.
     *
     * @param line 읽어온 한 줄입니다.
     */
    void    Process(const std::string& line);

public:
    Session(Channel* channel, int socketfd, int socketId, const std::string& addr);

    virtual ~Session();

    /**
     * @brief Channel에서 Read이벤트를 처리하게 될 메서드입니다.
     *
     */
    void    OnRead();

    /**
     * @brief Channel에서 Write이벤트를 처리하게 될 메서드입니다.
     *
     */
    void    OnWrite();

    /**
     * @brief 세션을 종료하고 싶을 때 호출합니다.
     *
     */
    void    Close();

    /**
     * @brief 현재 세션의 고유 ID를 반환합니다.
     *
     * @return 현재 세션의 고유 ID
     */
    int     GetSocketId() const;

    /**
     * @brief 현재 세션의 IP 주소를 반환합니다.
     *
     * @return 현재 세션의 IP 주소
     */
    const std::string&  GetRemoteAddress() const;

    /**
     * @brief 세션으로 문자열을 전송합니다. 문자열에는 여러 개행이 포함될 수 있으며, 클라이언트에서 이 개행을 각각 모두 분리하여 처리할 수도 있습니다.
     *
     * @param line 세션으로 전송될 문자열.
     */
    void    Send(const std::string& line);
};

#endif
