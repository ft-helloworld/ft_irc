/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 00:40:50 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 21:46:53 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <iosfwd>
#include <map>

class IRCSession;
class IRCMessage;

class IRCServer
{
private:
    typedef std::map<const std::string, int> ClientMap;
    const std::string _password;

    ClientMap _clients;

    IRCServer(const IRCServer&);
    IRCServer& operator= (const IRCServer&);

public:
    IRCServer(const std::string& password);
    ~IRCServer();

    void    OnNickname(IRCSession& session, IRCMessage& msg);
    void    OnUsername(IRCSession& session, IRCMessage& msg);
    void    OnPassword(IRCSession& session, IRCMessage& msg);

    void    UnregisterNickname(const std::string& nick);
    int     FindByNick(const std::string& nick) const;
    const std::string& GetPassword() const;
};

#endif
