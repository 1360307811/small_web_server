
# 小型web 服务器

## 项目描述:

      本项目为linux下 C++编写的小型Web服务器，处理静态资源，支持HTTP长连接，可以处理get、post请求，可并实现了日志，记录服务器运行状态 实现了定时器，来处理超时连接。
## 项目实现：
      1:首先进行了线程池的编程,在线程池的编程中使用了管理线程对线程池的大小进行了动态增加和缩减,核心是使用互斥量和条件变量来管理线程. 
      2:实现了一个小根堆的定时器及时剔除超时请求，使用了STL的优先队列来管理定时器，在处理请求时 验证请求是否超时 如果超时则将请求不处理 
      并踢出定时器
      3:epoll 模型部分的编程,在这部分编程中循环的等待连接事件和读写事件的发生,当事 件发生时,向线程池中增加相对应的函数,然后让线程池去
      处理这些事件
      4: 日志部分：仿照muduo，日志分为前端和后端 前端负责输入日志 后端则是个log线程循环的接受前端的数据。具体过程请看log目录下的log.txt
          
## 遇到问题:
         1. 如何设计各个模块的任务和联系  
         2.定时器和日志的设计和构造
## 解决方法：
          看书以及muduo网络库的源码可以明确设计思路，使用UML可以理清各模块间的联系
          log:
          ![Image text](https://github.com/1360307811/small_web_server/blob/master/image/log.png)
          server:
          ![Image text](https://github.com/1360307811/small_web_server/blob/master/image/server.png)
          
## 测试:
         webbench -c 100 -t 5:Speed=432800 pages/min,152655 bytes/sec. Requests: 36062, 0 failed.
## 总结
     通过此次项目让我对整个web 服务器的基本框架有了更深的理解,对epoll 模型的理解和使用以及整个 socket 编程流程, 还有对 TCP/IP/HTTP 协议部分有了初步的掌握.
      
