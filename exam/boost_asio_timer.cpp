#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <iostream>

using namespace boost::asio;

int asio_timer_demo(int argc, char* argv[])
{
  io_service ioservice;

  steady_timer timer1{ioservice, std::chrono::milliseconds{300}};
  timer1.async_wait([](const boost::system::error_code &ec)
    { std::cout << "300 milliseconds\n"; });

  steady_timer timer2{ioservice, std::chrono::milliseconds{400}};
  timer2.async_wait([](const boost::system::error_code &ec)
    { std::cout << "400 milliseconds\n"; });

  ioservice.run();
}