#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <iostream>
#include <thread>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string

using namespace boost::asio;

std::string current_time()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void do_task(const std::string& taskId) {
  std::cout << current_time() << " task " << taskId << " enter" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << current_time() << " task " << taskId << " leave" << std::endl;
}

int asio_timer_demo_1(int argc, char* argv[])
{
  io_service ioservice;

  steady_timer timer1{ioservice, std::chrono::milliseconds{300}};
  timer1.async_wait([](const boost::system::error_code &ec)
    { do_task("task1"); });

  steady_timer timer2{ioservice, std::chrono::milliseconds{400}};
  timer2.async_wait([](const boost::system::error_code &ec)
    { do_task("task2");  });

  ioservice.run();

  return 0;
}




int asio_timer_demo_2(int argc, char* argv[])
{
  io_service ioservice;

  steady_timer timer1{ioservice, std::chrono::seconds{5}};
  timer1.async_wait([](auto ... vn) { do_task("task1"); });

  steady_timer timer2{ioservice, std::chrono::seconds{5}};
  timer2.async_wait([](auto ... vn) { do_task("task2"); });

  std::thread butler([&]() { ioservice.run(); });
  butler.join();

  return 0;
}

int asio_timer_demo_3(int argc, char* argv[])
{
  io_service ioservice;

  steady_timer timer1{ioservice, std::chrono::seconds{5}};
  timer1.async_wait([](auto ... vn) { do_task("task1"); });

  steady_timer timer2{ioservice, std::chrono::seconds{5}};
  timer2.async_wait([](auto ... vn) { do_task("task2"); });

  std::thread ta([&]() { ioservice.run(); });
  std::thread tb([&]() { ioservice.run(); });
  ta.join();
  tb.join();

  return 0;
}

int asio_timer_demo_4(int argc, char* argv[])
{
  io_service ioservice;
  io_service::strand strand(ioservice);

  steady_timer timer1{ioservice, std::chrono::seconds{5}};
  timer1.async_wait(strand.wrap([](auto ... vn) { do_task("task1"); }));

  steady_timer timer2{ioservice, std::chrono::seconds{5}};
  timer2.async_wait(strand.wrap([](auto ... vn) { do_task("task2"); }));

  std::thread ta([&]() { ioservice.run(); });
  std::thread tb([&]() { ioservice.run(); });
  ta.join();
  tb.join();

  return 0;
}