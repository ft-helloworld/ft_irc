#include "ft_irc.hpp"

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        Log::I("Usage: %s [port] [password]", argv[0]);
        return 1;
    }

    try
    {
        Channel channel(6667);
        channel.Init();
        channel.BindAndListen();
        channel.Run();
    }
    catch (const std::exception& ex)
    {
        Log::F("Failed to start server. [%s]", ex.what());
        return 1;
    }
    return 0;
}
