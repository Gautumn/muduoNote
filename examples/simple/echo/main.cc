#include "examples/simple/echo/echo.h"

#include "muduo/net/EventLoop.h"
#include "muduo/base/Logging.h"

#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

int main()
{
  LOG_INFO << "pid = " << getpid();
  
  Logger::setLogLevel(Logger::TRACE);

  muduo::net::EventLoop loop;
  muduo::net::InetAddress listenAddr(2007);
  EchoServer server(&loop, listenAddr);
  server.start();
  loop.loop();
}

