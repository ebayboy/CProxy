#include <assert.h>
#include <sys/epoll.h>
#include <iostream>

#include "channel.h"
#include "epoll.h"
#include "spdlog/spdlog.h"

const int EPOLLWAIT_TIME = 10000;
const int EVENTSNUM = 4096;

Epoll::Epoll() : epoll_fd_(epoll_create1(EPOLL_CLOEXEC)), epoll_events_(EVENTSNUM) {
  assert(epoll_fd_ > 0);
}

void Epoll::PollAdd(SP_Channel channel) {
  int fd = channel->getFd();
  epoll_event event;
  event.data.fd = fd;
  event.events = channel->GetEvents();
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) < 0) {
    SPDLOG_CRITICAL("epoll_ctl fd: {} err: {}", fd, strerror(errno));
  } else {
    fd2chan_[fd] = channel;
  }
}

void Epoll::PollMod(SP_Channel channel) {
  int fd = channel->getFd();
  epoll_event event;
  event.data.fd = fd;
  event.events = channel->GetEvents();
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event) < 0) {
    perror("epoll_mod error");
    fd2chan_[fd].reset();
  }
}

void Epoll::PollDel(SP_Channel channel) {
  int fd = channel->getFd();
  epoll_event event;
  event.data.fd = fd;
  event.events = channel->GetEvents();
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &event) < 0) {
    perror("epoll_del error");
  }
  fd2chan_[fd].reset();
}

std::vector<SP_Channel> Epoll::WaitForReadyChannels() {
  for (;;) {
    // 取vector的首地址：
    // &*epoll_events_.begin() 解析： epoll_events_.begin()返回第一元素迭代器，
    // C++中，迭代器就是一个类似于指针的对象，它能够用来遍历C++标准模板库容器中的部分或全部元素，每个迭代器对象代表容器中的确定的地址。
    //*iter(返回迭代器所指元素的引用)
    // *epoll_events_.begin()代表第一个元素的引用，
    // &(*epoll_events_.begin()) : 对引用取地址实际是对原变量取地址， 既取第一个元素的地址
    int event_count =
        epoll_wait(epoll_fd_, &*epoll_events_.begin(), epoll_events_.size(), EPOLLWAIT_TIME);
    if (event_count < 0) {
      perror("epoll wait error");
      continue;
    }

    std::vector<SP_Channel> readyChannels = getReadyChannels(event_count);
    if (readyChannels.size() > 0) {
      return readyChannels;
    }
  }
}

std::vector<SP_Channel> Epoll::getReadyChannels(int event_count) {
  std::vector<SP_Channel> ret;

  for (int i = 0; i < event_count; i++) {
    epoll_event cur_event = epoll_events_[i];
    int fd = cur_event.data.fd;
    SP_Channel cur_chan = fd2chan_[fd];
    if (cur_chan) {
      cur_chan->SetRevents(cur_event.events);
      ret.emplace_back(cur_chan);
    } else {
      std::cout << "fd" << fd << "not exist in fd2chan_" << std::endl;
    }
  }
  return ret;
}