// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is an internal header file, you should not include this.

#ifndef MUDUO_NET_POLLER_EPOLLPOLLER_H
#define MUDUO_NET_POLLER_EPOLLPOLLER_H

#include "muduo/net/Poller.h"

#include <vector>

struct epoll_event;

namespace muduo
{
namespace net
{

///
/// IO Multiplexing with epoll(4).
///
class EPollPoller : public Poller
{
 public:
  EPollPoller(EventLoop* loop);
  ~EPollPoller() override;

  Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
  void updateChannel(Channel* channel) override;
  void removeChannel(Channel* channel) override;

 private:
  static const int kInitEventListSize = 16;

  static const char* operationToString(int op);

  void fillActiveChannels(int numEvents,
                          ChannelList* activeChannels) const;
  void update(int operation, Channel* channel);

  /**
   * epoll_event结构
   * struct epoll_event {
   *    __uint32_t events;  // Epoll events
   *    epoll_data_t data;  // User data variable
   * };
   * typedef union epoll_data
   * {
   *     void* ptr;              //指定与fd相关的用户数据
   *     int fd;                 //指定事件所从属的目标文件描述符
   *     uint32_t u32;
   *     uint64_t u64;
   * } epoll_data_t;
   */
  typedef std::vector<struct epoll_event> EventList;

  int epollfd_;
  EventList events_;
};

}  // namespace net
}  // namespace muduo
#endif  // MUDUO_NET_POLLER_EPOLLPOLLER_H
