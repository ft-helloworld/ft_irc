/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:43:03 by smun              #+#    #+#             */
/*   Updated: 2022/03/29 13:24:44 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "log.hpp"
#include <string>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <iomanip>

const char* Log::GetPrefix(int level)
{
    switch (level)
    {
        case LogLevel_Verbose:
            return "\033[34m[VERBOSE]";
        case LogLevel_Debug:
            return "\033[36m[DEBUG]";
        case LogLevel_Info:
            return "\033[37m[INFO]";
        case LogLevel_Warning:
            return "\033[33m[WARNING]";
        case LogLevel_Error:
            return "\033[35m[ERROR]";
        case LogLevel_Fatal:
            return "\033[31m[FATAL]";
        default:
            return "\033[0m";
    }
}

static void print_time()
{
    std::time_t	time = std::time(NULL);
    char        buffer[100];

    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time)))
        std::cout << buffer;
}

void    Log::Printf(int level, const char* format, va_list& va)
{
    Printfp(level, "", format, va);
}

void    Log::Printfp(int level, const char* prefix, const char* format, va_list& va)
{
    std::cout << "[";
    print_time();
    std::cout << "] ";
    std::cout << std::setw(5 + 9) << std::left << GetPrefix(level);
    std::cout << " [";
    std::cout << std::setw(22) << std::left << prefix;
    std::cout << "] ";
    std::vfprintf(stdout, format, va);
    std::fflush(stdout);
    std::cout << GetPrefix(-1);
    std::cout << std::endl;
}

#define VBEGIN  va_list va; va_start(va, format)
#define VEND    va_end(va)

void    Log::V(const char* format, ...) { VBEGIN; Printf(LogLevel_Verbose, format, va); VEND; }
void    Log::D(const char* format, ...) { VBEGIN; Printf(LogLevel_Debug, format, va); VEND; }
void    Log::I(const char* format, ...) { VBEGIN; Printf(LogLevel_Info, format, va); VEND; }
void    Log::W(const char* format, ...) { VBEGIN; Printf(LogLevel_Warning, format, va); VEND; }
void    Log::E(const char* format, ...) { VBEGIN; Printf(LogLevel_Error, format, va); VEND; }
void    Log::F(const char* format, ...) { VBEGIN; Printf(LogLevel_Fatal, format, va); VEND; }

void    Log::Vp(const char* prefix, const char* format, ...) { VBEGIN; Printfp(LogLevel_Verbose, prefix, format, va); VEND; }
void    Log::Dp(const char* prefix, const char* format, ...) { VBEGIN; Printfp(LogLevel_Debug, prefix, format, va); VEND; }
void    Log::Ip(const char* prefix, const char* format, ...) { VBEGIN; Printfp(LogLevel_Info, prefix, format, va); VEND; }
void    Log::Wp(const char* prefix, const char* format, ...) { VBEGIN; Printfp(LogLevel_Warning, prefix, format, va); VEND; }
void    Log::Ep(const char* prefix, const char* format, ...) { VBEGIN; Printfp(LogLevel_Error, prefix, format, va); VEND; }
void    Log::Fp(const char* prefix, const char* format, ...) { VBEGIN; Printfp(LogLevel_Fatal, prefix, format, va); VEND; }
