

## 启动server

### 启动：
root@502f7b062706 ~/g/CProxy (main) [SIGINT]# ./run_server.sh
ctl_port:8080; proxy_port:8089; work_thread_nums: 1


### 查看端口
* 8080为控制端口， client连接server， 向注册服务
* 8089为反向代理端口， 负责转发数据
root@502f7b062706 ~/g/CProxy (main)# netstat -anpt | grep Server
tcp        0      0 0.0.0.0:8080            0.0.0.0:*               LISTEN      2041/./Server
tcp        0      0 0.0.0.0:8089            0.0.0.0:*               LISTEN      2041/./Server


## 启动client
root@502f7b062706 ~/g/CProxy (main)# bash -x ./run_client.sh
* ./Client --local_server=127.0.0.1:7777 --cproxy_server=127.0.0.1:8080
[/root/github.com/CProxy/client/Client.cpp:68 07:49:09:913 +00:00] [I] [thread 2141] tunnel addr:0.0.0.0:37441

### 源站： --local_server=127.0.0.1:7777
server地址：  --cproxy_server=127.0.0.1:8080

### 查看端口
tcp        0      0 0.0.0.0:8080            0.0.0.0:*               LISTEN      2178/./Server   //控制监听端口
tcp        0      0 0.0.0.0:8089            0.0.0.0:*               LISTEN      2178/./Server   //数据转发端口
tcp        0      0 0.0.0.0:34369           0.0.0.0:*               LISTEN      2178/./Server   //client注册的127.0.0.1:7777服务对外的服务端口
tcp        0      0 127.0.0.1:8080          127.0.0.1:34706         ESTABLISHED 2178/./Server   //client与server已建立的连接

## 请求server对外发布的端口
可以看到通过serverd代理返回了源站7777的内容
```
root@502f7b062706 /v/w/html# curl http://0.0.0.0:34369/index.html
 <head>
  <title>I'm from 7777</title>
 </head>
 <body>
  <h1>I'm from 7777</h1>
 </body>
</html>
```
