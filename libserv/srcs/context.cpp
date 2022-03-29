/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:18:07 by smun              #+#    #+#             */
/*   Updated: 2022/03/29 17:45:49 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "std.hpp"
#include "context.hpp"
#include "channel.hpp"
#include "log.hpp"
#include <unistd.h>

Context::Context(Channel* channel, int fd)
    : _socketfd(fd)
    , _attachedChannel(channel)
{
}

Context::~Context()
{
    close(_socketfd);
    Log::Vp("Context::~Context", "[%d] 컨텍스트 인스턴스가 삭제됩니다.", _socketfd);
}

int Context::GetSocket() const
{
    return _socketfd;
}
