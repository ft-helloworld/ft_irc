/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yejsong <yejsong@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 14:37:44 by smun              #+#    #+#             */
/*   Updated: 2022/03/30 20:22:57 by yejsong          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_HPP
#define STRING_HPP

#include <vector>
#include <iosfwd>

class String
{
private:
    String();
    String(const String&);
    String& operator= (const String&);

public:
    ~String();

    typedef std::vector<const std::string> StringVector;

    static void SplitArguments(std::vector<const std::string>& args, const std::string& line);
    static int  Stoi(const std::string& str);
    static std::string Join(
        std::vector<const std::string>::const_iterator begin,
        std::vector<const std::string>::const_iterator end);
    static bool IsLetter(char ch);
    static bool IsDigit(char ch);
    static bool IsSpecial(char ch);
};

#endif
