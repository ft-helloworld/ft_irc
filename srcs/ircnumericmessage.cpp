/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircnumericmessage.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 17:08:20 by smun              #+#    #+#             */
/*   Updated: 2022/04/02 15:56:47 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "ircnumericmessage.hpp"

IRCNumericMessage::IRCNumericMessage(const IRCNumericMessage& o)
    : IRCMessage(o) {}
IRCNumericMessage::~IRCNumericMessage() {}

IRCNumericMessage::IRCNumericMessage(int cmd, const std::string& p1)
    : IRCMessage(HOSTNAME, cmd, p1) {}

IRCNumericMessage::IRCNumericMessage(int cmd, const std::string& p1, const std::string& p2)
    : IRCMessage(HOSTNAME, cmd, p1, p2)
{
}

IRCNumericMessage::IRCNumericMessage(int cmd, const std::string& p1, const std::string& p2, const std::string& p3)
    : IRCMessage(HOSTNAME, cmd, p1, p2, p3)
{
}

IRCNumericMessage::IRCNumericMessage(int cmd, const std::string& p1, const std::string& p2, const std::string& p3, const std::string& p4)
    : IRCMessage(HOSTNAME, cmd, p1, p2, p3)
{
    AddParam(p4);
}

IRCNumericMessage::IRCNumericMessage(int cmd, const std::string& p1, const std::string& p2, const std::string& p3, const std::string& p4, const std::string& p5)
    : IRCMessage(HOSTNAME, cmd, p1, p2, p3)
{
    AddParam(p4);
    AddParam(p5);
}
