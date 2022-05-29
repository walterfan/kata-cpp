#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <iostream>
#include <thread>


using namespace boost::asio;


void task(std::string taskId) {
  std::cout << "task " << taskId << " enter" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "task " << taskId << " lave" << std::endl;
}


int asio_timer_demo_2(int argc, char* argv[])
{
  io_service ioservice;

  steady_timer timer1{ioservice, std::chrono::milliseconds{300}};
  timer1.async_wait([](auto ... vn) { task("task1"); });

  steady_timer timer2{ioservice, std::chrono::milliseconds{400}};
  timer2.async_wait([](auto ... vn) { task("task2"); });

  std::thread butler([&]() { ioservice.run(); });
  butler.join();
  std::cout << "Done." << std::endl;

  return 0;
}