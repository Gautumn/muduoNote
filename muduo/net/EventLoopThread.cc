// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "muduo/net/EventLoopThread.h"

#include "muduo/net/EventLoop.h"

using namespace muduo;
using namespace muduo::net;

/// 传进来的name是char类型的，这里有一个char->string隐式转换
EventLoopThread::EventLoopThread(const ThreadInitCallback& cb,
                                 const string& name)
  : loop_(NULL),
    exiting_(false),
    thread_(std::bind(&EventLoopThread::threadFunc, this), name),
    mutex_(),
    cond_(mutex_),
    callback_(cb)
{
}

EventLoopThread::~EventLoopThread()
{
  exiting_ = true;
  if (loop_ != NULL) // not 100% race-free, eg. threadFunc could be running callback_.
  {
    // still a tiny chance to call destructed object, if threadFunc exits just now.
    // but when EventLoopThread destructs, usually programming is exiting anyway.
    loop_->quit();
    thread_.join();
  }
}

EventLoop* EventLoopThread::startLoop()
{
  assert(!thread_.started());
  /// 在thread_初始化的时候传进来了一个线程函数: thread_(std::bind(&EventLoopThread::threadFunc, this), name)
  /// 创建并启动线程，线程的内容就是当前类的threadFunc()函数
  /// thread.cc中对传进来的线程函数会做统一的异常的处理。
  
  /// 创建 pthread_create()线程，该线程函数会调用传进来的 threadFunc()
  thread_.start();

  /// 等待threadFunc()调用完成
  EventLoop* loop = NULL;
  {
    MutexLockGuard lock(mutex_);
    while (loop_ == NULL)
    {
      cond_.wait();
    }
    loop = loop_;
  }

  /// 返回当前正在使用的eventloop
  return loop;
}

void EventLoopThread::threadFunc()
{
  EventLoop loop;

  if (callback_)
  {
    callback_(&loop);
  }

  {
    MutexLockGuard lock(mutex_);
    loop_ = &loop;
    cond_.notify();
  }

  /// 事件循环，EventLoop->loop();阻塞等待事件
  loop.loop();

  /// 事件结束
  //assert(exiting_);
  MutexLockGuard lock(mutex_);
  loop_ = NULL;
}
