/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 14:37:44 by smun              #+#    #+#             */
/*   Updated: 2022/04/07 20:54:22 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_HPP
#define STRING_HPP

#include <vector>
#include <string>
#include <sstream>
#include <type_traits>

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

    template<typename T>
    static const std::string ItoString(T number, typename std::enable_if<std::is_integral<T>::value>::type* = NULL)
    {
        std::ostringstream ss;
        ss << number;
        return ss.str();
    }

    static bool IsLetter(char ch);
    static bool IsDigit(char ch);
    static bool IsSpecial(char ch);

    template<typename InputIterator>
    static const std::string Join(InputIterator first, InputIterator last, const std::string& delimeter)
    {
        std::ostringstream oss;
        InputIterator it = first;

        while (it != last)
        {
            if (it != first)
                oss << delimeter;
            oss << *it;
            ++it;
        }
        return oss.str();
    }
};

#endif
