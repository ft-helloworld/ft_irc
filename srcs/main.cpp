#include "ft_irc.hpp"
#include "ircserver.hpp"
#include "ircsessionfactory.hpp"
#include "ircbot.hpp"
#include <cstdlib>
#include <stdexcept>
#include <signal.h>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        Log::I("Usage: %s [port] [password]", argv[0]);
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);

    try
    {
        std::srand(0);

        IRCServer   server(argv[2]);
        IRCSessionFactory sessionFactory(&server);

        // 입력받은 포트 번호를 정수로 변환하고, 변환할 수 없었다면 예외 발생!
        int port;
        try
        {
            port = String::Stoi(argv[1]);
        }
        catch (const std::exception& ex)
        {
            throw std::runtime_error("Typed port number is not integer.");
        }
        if (port < 1 || port > 65535)
            throw std::runtime_error("Not valid port range");

        IRCBot bot(&server, BOTNAME, BOTNAME);
        server.RegisterBot(bot);

        Channel channel(port, &sessionFactory);
        channel.Init();
        channel.BindAndListen();
        channel.AddTimer(&server);
        channel.Run();
    }
    catch (const std::exception& ex)
    {
        Log::F("Failed to start server. [%s]", ex.what());
        return 1;
    }
    return 0;
}
