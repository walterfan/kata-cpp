#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <chrono>
#include <iostream>

using namespace boost::asio;

int asio_chat_server(int argc, char* argv[])
{
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: chat_server <port> [<port> ...]\n";
            return 1;
        }

        boost::asio::io_context io_context(1);

        for (int i = 1; i < argc; ++i)
        {
            unsigned short port = std::atoi(argv[i]);
            co_spawn(io_context,
                listener(tcp::acceptor(io_context, {tcp::v4(), port})),
                detached);
        }

        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto){ io_context.stop(); });

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

int asio_chat_client(int argc, char* argv[])
{
    //TBD
    return 0;
}

int asio_udp_demo(int argc, char* argv[])
{
    std::cout << "TBD..." << std::endl;
    return 0;
}


