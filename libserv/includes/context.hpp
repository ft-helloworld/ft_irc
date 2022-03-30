/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:09:45 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 00:31:26 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <vector>

class Channel;

class Context
{
private:
    const int       _socketfd;

    Context();
    Context& operator= (const Context&);
    Context(const Context& ctx);

protected:
    /**
     * @brief 현재 Context에 연결된 채널의 포인터입니다.
     *
     */
    Channel* const  _attachedChannel;
    bool            _closed;

public:
    /**
     * @brief Context 객체를 소멸합니다. 여기에서 _socketfd의 close 호출이 이루어집니다.
     *
     */
    virtual ~Context();

    /**
     * @brief 새로운 Context 객체를 생성합니다.
     *
     * @param channel 이 Context에 연결될 channel입니다.
     * @param fd 이 Context에 할당될 소켓 fd입니다.
     */
    Context(Channel* channel, int fd);

    /**
     * @brief 현재 Context의 소켓 fd를 얻어옵니다.
     *
     * @return 현재 Context의 소켓 fd.
     */
    int     GetSocket() const;

    /**
     * @brief 현재 소켓 fd가 닫혔는지 여부입니다. listen소켓의 경우, 서버가 종료될 때 닫힘이 트리거되므로, 항상 false로 간주합니다.
     *
     * @return 현재 소켓 fd가 닫힌 여부입니다.
     */
    bool    IsClosed() const;
};

#endif
