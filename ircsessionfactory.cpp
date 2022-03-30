/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircsessionfactory.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 15:04:11 by smun              #+#    #+#             */
/*   Updated: 2022/03/30 15:09:17 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

IRCSessionFactory::IRCSessionFactory() {}
IRCSessionFactory::~IRCSessionFactory() {}
IRCSessionFactory::IRCSessionFactory(const IRCSessionFactory&) {}
IRCSessionFactory& IRCSessionFactory::operator=(const IRCSessionFactory&) { return *this; }

Session*
IRCSessionFactory::CreateSession(Channel* channel, int socketfd, int socketId, const std::string& addr)
{
    return new IRCSession(channel, socketfd, socketId, addr);
}
