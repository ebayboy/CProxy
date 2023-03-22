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

class Server : public std::enable_shared_from_this<Server> {
 public:
  Server(int threadNum, int ctlPort, int proxyPort);
  void start();
  std::unordered_map<std::string, SP_Control> control_map_;
  SP_EventLoopThreadPool eventLoopThreadPool_;
  SP_EventLoopThread publicListenThread_;
  int getProxyPort() { return proxyPort_; };
  UnclaimedProxyMap* getUnclaimedProxyMapByFd(int fd);

 private:
  int ctlPort_;
  int proxyPort_;
  int ctlListenFd_;
  int proxyListenFd_;
  SP_EventLoop loop_;
  SP_Channel ctl_acceptor_;
  SP_Channel proxy_acceptor_;
  UnclaimedProxyMap* hashedUnclaimedProxyMaps[UnclaimedProxyMapLen];
  void newCtlConnHandler();
  void newProxyConnHandler();
  void postHandler();
  void claimProxyConn(void*, SP_ProxyConn);
};
