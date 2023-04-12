#pragma once
#include <memory>
#include <vector>
#include "channel.h"

/// poller接口文件

class EventDispatcher {
 public:
  EventDispatcher(){};
  virtual ~EventDispatcher(){};  /// 析构函数是virtual的，动态绑定，派生类的内存释放
  virtual void PollAdd(
      SP_Channel) = 0;  /// 纯虚函数： 派生类需实现接口, SP_Channel = shared_ptr<Channel>
  virtual void PollMod(SP_Channel) = 0;
  virtual void PollDel(SP_Channel) = 0;
  virtual std::vector<SP_Channel> WaitForReadyChannels() = 0;
};

using SP_EventDispatcher =
    std::shared_ptr<EventDispatcher>;  /// 别名： using xxx  = yyy， shared_ptr： 使用智能指管理内存