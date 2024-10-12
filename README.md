# MyWeb

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
