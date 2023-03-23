#pragma once
#include <sys/epoll.h>
#include <functional>
#include <iostream>
#include <mutex>
#include <unordered_map>

#include "control.h"
#include "lib/ctl_conn.h"
#include "lib/event_loop.h"
#include "lib/event_loop_thread.h"
#include "lib/event_loop_thread_pool.h"
#include "lib/proxy_conn.h"
#include "tunnel.h"

const int UnclaimedProxyMapLen = 4;
struct UnclaimedProxyMap {                      // TODO: Unclaimed - 无人认领的？
  std::unordered_map<int, SP_ProxyConn> conns;  // unordered_map<key, value>  <==> hash_map
  std::mutex mutex;
};

// 继承enable_shared_from_this 的基类，通过shared_from_this（）返回shared_ptr指针，
// 对指针复制操作不会造成内存二次释放
class Server : public std::enable_shared_from_this<Server> {
 public:
  Server(int threadNum, int ctlPort, int proxyPort);
  void start();
  // 存放控制器的hash map
  std::unordered_map<std::string, SP_Control> control_map_;

  // 事件处理线程池
  SP_EventLoopThreadPool eventLoopThreadPool_;

  // 事件处理线程
  SP_EventLoopThread publicListenThread_;
  int getProxyPort() { return proxyPort_; };
  UnclaimedProxyMap* getUnclaimedProxyMapByFd(int fd);

 private:
  int ctlPort_;    // ctlPort <-> client
  int proxyPort_;  // proxyPort_ <-> client for data transmission
  int ctlListenFd_;
  int proxyListenFd_;
  SP_EventLoop loop_;  // SP:  shared_ptr
  SP_Channel ctl_acceptor_;
  SP_Channel proxy_acceptor_;
  UnclaimedProxyMap* hashedUnclaimedProxyMaps[UnclaimedProxyMapLen];
  void newCtlConnHandler();
  void newProxyConnHandler();
  void postHandler();
  void claimProxyConn(void*, SP_ProxyConn);  // claim: 认领
};
