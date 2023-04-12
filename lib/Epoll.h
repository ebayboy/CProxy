#pragma once
#include <sys/epoll.h>
#include <memory>
#include <vector>
#include "channel.h"
#include "event_dispatcher.h"

class Epoll : public EventDispatcher {  /// 类Epoll实现接口EventDispatcher
 public:
  Epoll();
  virtual ~Epoll(){};
  virtual void PollAdd(
      SP_Channel) override final;  /// override：重写基类方法； final无法被子类继续重写
  virtual void PollMod(SP_Channel)
      override final;  /// SP_Channel是类型， 函数声明时可以没有形参， 但定义时必须有形参
  virtual void PollDel(SP_Channel) override final;
  virtual std::vector<SP_Channel> WaitForReadyChannels() override final;
  int get_fd() { return epoll_fd_; }

 private:
  std::vector<SP_Channel> getReadyChannels(int);
  static const int MAXFDS = 100000;
  int epoll_fd_;
  SP_Channel fd2chan_[MAXFDS];
  std::vector<epoll_event> epoll_events_;
};

typedef std::shared_ptr<Epoll> SP_Epoll;  /// 推荐改写为 using SP_Epoll = std::shared_ptr<Epoll>