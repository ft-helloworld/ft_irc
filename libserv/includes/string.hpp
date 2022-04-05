/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yejsong <yejsong@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 14:37:44 by smun              #+#    #+#             */
/*   Updated: 2022/04/05 20:24:48 by yejsong          ###   ########.fr       */
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

    static void SplitArguments(StringVector& args, const std::string& line);
    static int  Stoi(const std::string& str);
    static const std::string ItoCode(int number);
    static const std::string ItoString(int number);
    static bool IsLetter(char ch);
    static bool IsDigit(char ch);
    static bool IsSpecial(char ch);
};

#endif
