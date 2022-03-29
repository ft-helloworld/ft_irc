/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/24 15:38:55 by smun              #+#    #+#             */
/*   Updated: 2022/03/29 17:45:55 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP
#define LOG_HPP

#include <iosfwd>
#include <string> // for convenience
#include <cstdarg>

class Log
{
private:
    enum
    {
        LogLevel_Verbose,
        LogLevel_Debug,
        LogLevel_Info,
        LogLevel_Warning,
        LogLevel_Error,
        LogLevel_Fatal
    };

    Log();
    ~Log();
    Log(const Log&);
    Log& operator=(const Log&);

    static const char* GetPrefix(int level);
    static void Printf(int level, const char* format, va_list& va);
    static void Printfp(int level, const char* prefix, const char* format, va_list& va);

public:
    /**
     * @brief VERBOSE 등급으로 로그를 출력합니다. 가장 낮은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    static void V(const char* format, ...);

    /**
     * @brief DEBUG 등급으로 로그를 출력합니다. 2번째로 낮은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    static void D(const char* format, ...);

    /**
     * @brief INFO 등급으로 로그를 출력합니다. 3번째로 낮은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    static void I(const char* format, ...);

    /**
     * @brief WARNING 등급으로 로그를 출력합니다. 3번째로 높은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    static void W(const char* format, ...);

    /**
     * @brief ERROR 등급으로 로그를 출력합니다. 2번째로 높은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    static void E(const char* format, ...);

    /**
     * @brief FATAL 등급으로 로그를 출력합니다. 가장 높은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    static void F(const char* format, ...);

    /**
     * @brief VERBOSE 등급으로 Prefix 열에 내용을 채워 로그를 출력합니다. 가장 낮은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param prefix 로그 앞에 붙일 머릿말
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    static void Vp(const char* prefix, const char* format, ...);

    /**
     * @brief DEBUG 등급으로 Prefix 열에 내용을 채워 로그를 출력합니다. 2번째로 낮은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param prefix 로그 앞에 붙일 머릿말
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    static void Dp(const char* prefix, const char* format, ...);

    /**
     * @brief INFO 등급으로 Prefix 열에 내용을 채워 로그를 출력합니다. 3번째로 낮은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param prefix 로그 앞에 붙일 머릿말
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    static void Ip(const char* prefix, const char* format, ...);

    /**
     * @brief WARNING 등급으로 Prefix 열에 내용을 채워 로그를 출력합니다. 3번째로 높은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param prefix 로그 앞에 붙일 머릿말
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    static void Wp(const char* prefix, const char* format, ...);

    /**
     * @brief ERROR 등급으로 Prefix 열에 내용을 채워 로그를 출력합니다. 2번째로 높은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param prefix 로그 앞에 붙일 머릿말
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    static void Ep(const char* prefix, const char* format, ...);

    /**
     * @brief FATAL 등급으로 Prefix 열에 내용을 채워 로그를 출력합니다. 가장 높은 로깅 등급입니다. printf와 사용법이 동일합니다.
     *
     * @param prefix 로그 앞에 붙일 머릿말
     * @param format 형식 타입
     * @param ... 메시지 인자
     */
    static void Fp(const char* prefix, const char* format, ...);
};

#endif
