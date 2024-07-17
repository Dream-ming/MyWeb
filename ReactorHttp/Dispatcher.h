#pragma once
#include "Channel.h"
#include "EventLoop.h"

struct Dispatcher
{
    // init -- 初始化epoll，poll或select需要的数据库
    void* (*init)();
    // 添加
    int (*add)(struct Channel* channel, struct EventLoop* evLoop);
    // 删除
    int (*remove)(struct Channel* channel, struct EventLoop* evLoop);
    // 修改
    int (*modify)(struct Channel* channel, struct EventLoop* evLoop);
    // 事件检测
    int (*dispatch)(struct EventLoop* evLoop, int timeout); // 单位：s
    // 清除数据(关闭fd或释放内存)
    int (*clear)(struct EventLoop* evLoop);
};