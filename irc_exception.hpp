/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_exception.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 00:41:50 by smun              #+#    #+#             */
/*   Updated: 2022/04/01 19:32:41 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_EXCEPTION_HPP
#define IRC_EXCEPTION_HPP

#include <string>
#include "ircmessage.hpp"

struct IRCCommand;

class irc_exception
{
private:
    IRCMessage _message;

    irc_exception();
    irc_exception& operator= (const irc_exception&);

public:
    irc_exception(const irc_exception&);
    virtual ~irc_exception() throw();
    irc_exception(const IRCCommand& cmd) throw();
    irc_exception(const IRCCommand& cmd, const std::string& trailing) throw();
    irc_exception(const IRCCommand& cmd, const std::string& param1, const std::string& trailing) throw();
    irc_exception(const IRCCommand& cmd, const std::string& param1, const std::string& param2, const std::string& trailing) throw();
    irc_exception(const IRCCommand& cmd, const std::string& param1, const std::string& param2, const std::string& param3, const std::string& trailing) throw();
    irc_exception(const IRCCommand& cmd, const std::string& param1, const std::string& param2, const std::string& param3, const std::string& param4, const std::string& trailing) throw();

    const IRCMessage& message() const throw();
};

#endif
