/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_exception.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 00:41:50 by smun              #+#    #+#             */
/*   Updated: 2022/03/31 17:05:09 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_EXCEPTION_HPP
#define IRC_EXCEPTION_HPP

#include <string>
#include <exception>

class irc_exception : public std::exception
{
private:

    std::string _message;
    irc_exception();
    irc_exception& operator= (const irc_exception&);

public:
    irc_exception(const irc_exception&);
    virtual ~irc_exception() throw();
    irc_exception(int cmd) throw();
    irc_exception(int cmd, const std::string& trailing) throw();
    irc_exception(int cmd, const std::string& trailing, const std::string& param1) throw();
    irc_exception(int cmd, const std::string& trailing, const std::string& param1, const std::string& param2) throw();
    irc_exception(int cmd, const std::string& trailing, const std::string& param1, const std::string& param2, const std::string& param3) throw();
    irc_exception(int cmd, const std::string& trailing, const std::string& param1, const std::string& param2, const std::string& param3, const std::string& param4) throw();

    const char* what() const throw();
};

#endif
