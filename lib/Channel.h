#pragma once
#include <unistd.h>
#include <functional>
#include <memory>

class ChannelOwner {};

/// using SP_ChannelOwner = std::shared_ptr<ChannelOwner>
typedef std::shared_ptr<ChannelOwner> SP_ChannelOwner;

class Channel {
 public:
  /// using EventHandler = std::function<void()>
  typedef std::function<void()> EventHandler;
  Channel(int fd) : fd_(fd), need_close_when_delete_(true){};
  ~Channel() {
    if (need_close_when_delete_) {
      close(fd_);
    }
  }

  void setFd(int fd) { fd_ = fd; };
  int getFd() { return fd_; };

  void SetEvents(__uint32_t events) { events_ = events; };
  void SetRevents(__uint32_t events) { revents_ = events; };
  void AddEvents(__uint32_t events) { events_ |= events; }
  __uint32_t GetEvents() { return events_; }
  __uint32_t GetRevents() { return revents_; }

  void SetReadHandler(EventHandler handler) { read_handler_ = handler; };
  void SetWriteHandler(EventHandler handler) { write_handler_ = handler; };
  void SetErrorHandler(EventHandler handler) { error_handler_ = handler; }
  void SetPostHandler(EventHandler handler) { post_handler_ = handler; }
  void SetChannelOwner(SP_ChannelOwner owner) { belong_to_ = owner; }

  void HandleEvents();
  bool IsPeerClosed() { return peer_closed_; }
  void SetNeedCloseWhenDelete(bool needClose) { need_close_when_delete_ = needClose; }

 private:
  int fd_;  // 对应epoll event的fd
  bool need_close_when_delete_;
  __uint32_t events_;                    // 对应epoll event的events_
  __uint32_t revents_;                   // TODO: 读事件？
  EventHandler read_handler_ = []() {};  /// 默认值是空lambda函数
  EventHandler error_handler_ = []() {};
  EventHandler write_handler_ = []() {};
  // 每次事件处理后，对当前channel的事件更新或将当前channel从epoll中去掉
  EventHandler post_handler_ = []() {};
  SP_ChannelOwner belong_to_;  // 空类：占用空间就会分配地址， 可以通过地址区分对象
  bool peer_closed_;
};

typedef std::shared_ptr<Channel> SP_Channel;  /// using SP_Channel = std::shared_ptr<Channel>