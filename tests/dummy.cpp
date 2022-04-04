/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dummy.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 13:59:10 by smun              #+#    #+#             */
/*   Updated: 2022/04/05 01:17:11 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <mutex>
#include <unistd.h>
#include <signal.h>

const char *const CRLF = "\r\n";
size_t CRLF_SIZE = std::strlen(CRLF);

struct TestContext
{
    int threadNum;
    std::string host;
    int port;
    std::string password;
    sockaddr_in addr;

    std::mutex globalMutex;
    int threadIdCounter = 0;
};

static void _send_packet(int sock, const std::string& line)
{
    ssize_t ret = send(sock, line.c_str(), line.length(), 0);
    ssize_t ret2 = send(sock, CRLF, CRLF_SIZE, 0);
    if (ret < 0 || ret2 < 0)
        throw 0;
}

static const std::string _recv_packet(std::vector<char>& recvBuffer, int sock)
{
    char buffer[1460];

    ssize_t bytes = recv(sock, buffer, sizeof(buffer), 0);
    if (bytes < 0)
        throw std::runtime_error("disconnected");
    recvBuffer.reserve(recvBuffer.size() + bytes);
    recvBuffer.insert(recvBuffer.end(), buffer, buffer + bytes);

    std::vector<char>::iterator found = std::search(recvBuffer.begin(), recvBuffer.end(), CRLF, CRLF + CRLF_SIZE);
    if (found != recvBuffer.end())
    {
        std::string line(recvBuffer.begin(), found);
        recvBuffer.erase(recvBuffer.begin(), found + CRLF_SIZE);
        return line;
    }
    return std::string();
}

static void* _run(void* pctx)
{
    TestContext* ctx = reinterpret_cast<TestContext*>(pctx);
    sockaddr* addr = reinterpret_cast<sockaddr*>(&ctx->addr);

    int sock;
    while (true)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            std::perror("can't create socket");
            continue;
        }
        int ret = connect(sock, addr, sizeof(*addr));
        if (ret < 0)
        {
            if (errno == ECONNRESET)
            {
                printf("connection failed.. retry\n");
                continue;
            }
            std:perror("can't connect to host.");
            return NULL;
        }
        break;
    }

    int threadId;
    {
        std::lock_guard<std::mutex>(ctx->globalMutex);
        threadId = ++(ctx->threadIdCounter);
        printf("thread-%d begins test\n", threadId);
    }

    int chatNum;

    std::vector<char> recvBuffer;
    try
    {
        _send_packet(sock, std::string("PASS ") + ctx->password);
        _send_packet(sock, std::string("NICK tbot") + std::to_string(threadId));
        _send_packet(sock, std::string("USER tbot") + std::to_string(threadId) + " 0 * :bot test");
        _send_packet(sock, "JOIN #42,#42Seoul,#fourtytwo,#안녕,#42서울");
        while (true)
        {
            _send_packet(sock, "PRIVMSG #42 :Chat-" + std::to_string(chatNum++));
            _send_packet(sock, "PRIVMSG #42Seoul :Chat-" + std::to_string(chatNum++));
            _send_packet(sock, "PRIVMSG #fourtytwo :Chat-" + std::to_string(chatNum++));
            _send_packet(sock, "PRIVMSG #안녕 :Chat-" + std::to_string(chatNum++));
            _send_packet(sock, "PRIVMSG #42서울 :Chat-" + std::to_string(chatNum++));
            _send_packet(sock, "PRIVMSG #42,#42Seoul,#fourtytwo,#안녕,#42서울 :Chat-" + std::to_string(chatNum++));
            usleep(200000);
        }
    }
    catch (...)
    {
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    signal(SIGPIPE, SIG_IGN);

    TestContext ctx;

    ctx.threadNum = 1;
    ctx.host = "127.0.0.1";
    ctx.port = 6667;
    ctx.password = "password";

    if (argc >= 2)
        ctx.threadNum = std::atoi(argv[1]);
    if (argc >= 3)
        ctx.host = argv[2];
    if (argc >= 4)
        ctx.port = std::atoi(argv[3]);
    if (argc >= 5)
        ctx.password = argv[4];

    std::memset(&ctx.addr, 0, sizeof(ctx.addr));
    ctx.addr.sin_family = AF_INET;
    ctx.addr.sin_port = htons(ctx.port);

    inet_pton(AF_INET, ctx.host.c_str(), &ctx.addr.sin_addr);

    std::vector<pthread_t> threads;
    for (int i = 0; i < ctx.threadNum; ++i)
    {
        pthread_t t;
        pthread_create(&t, NULL, _run, &ctx);
        threads.push_back(t);
        usleep(100000);
    }
    printf("dummy is working... \n");

    for (const auto& thread : threads)
        pthread_join(thread, NULL);

    printf("all dummy died \n");

    return 0;
}
