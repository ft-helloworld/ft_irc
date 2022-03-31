/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 00:40:50 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 18:37:08 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <iosfwd>
#include <map>

class IRCSession;

class IRCServer
{
private:
    std::map<const std::string, int> _clients;

    IRCServer(const IRCServer&);
    IRCServer& operator= (const IRCServer&);

public:
    IRCServer();
    ~IRCServer();

    void    OnNickname(IRCSession& session, const std::string& beforeNick, const std::string& afterNick);
    void    OnUsername(IRCSession& session);
    void    UnregisterNickname(const std::string& nick);
};

#endif
