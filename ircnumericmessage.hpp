/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircnumericmessage.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 17:05:07 by smun              #+#    #+#             */
/*   Updated: 2022/04/01 17:12:27 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCNUMERICMESSAGE_HPP
#define IRCNUMERICMESSAGE_HPP

#include "ircmessage.hpp"

class IRCNumericMessage : public IRCMessage
{
private:
    IRCNumericMessage& operator= (const IRCNumericMessage&);
    IRCNumericMessage();

public:
    IRCNumericMessage(const IRCNumericMessage& o);
    IRCNumericMessage(int cmd, const std::string& trailing);
    IRCNumericMessage(int cmd, const std::string& p1, const std::string& trailing);
    IRCNumericMessage(int cmd, const std::string& p1, const std::string& p2, const std::string& trailing);
    IRCNumericMessage(int cmd, const std::string& p1, const std::string& p2, const std::string& p3, const std::string& trailing);
    IRCNumericMessage(int cmd, const std::string& p1, const std::string& p2, const std::string& p3, const std::string& p4, const std::string& trailing);
    IRCNumericMessage(int cmd, const std::string& p1, const std::string& p2, const std::string& p3, const std::string& p4, const std::string& p5, const std::string& trailing);

    virtual ~IRCNumericMessage();
};

#endif
