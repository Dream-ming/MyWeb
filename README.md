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
* 实现Proactor模型
* 日志系统
* 消息队列
* 分布式
