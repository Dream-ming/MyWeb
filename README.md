# MyWeb

这里是跟着网上做的一个服务器项目。

这里放了三个版本
* simple版：仅实现了最简单的通信功能
* reactor版：实现了Reactor高并发模型
* cpp版，将reactor版从C重构成C++

本项目开发了一个基于Linux平台的轻量级HTTP服务器
* 采用了Reactor高并发模型
* 支持Select、Poll、Epoll三种IO复用方式
* 服务器通过状态机解析HTTP请求报文
* 使用自动增长的缓冲区来提升性能稳定性
* 通过WebBench对服务器进行了性能和压力测试，确保了其稳定性和可靠性。
