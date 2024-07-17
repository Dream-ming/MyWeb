#pragma once
#include <stdbool.h>
#include <pthread.h>
#include "Dispatcher.h"
#include "ChannelMap.h"

// 跨文件调用数据
extern struct Dispatcher EpollDispatcher;
extern struct Dispatcher PollDispatcher;
extern struct Dispatcher SelectDispatcher;

// 处理改节点中的channel的方式
enum ElemType{ADD, DELETE, MODIFY};
// 定义任务队列的节点
struct ChannelElement
{
    int type; // 处理channel的方式
    struct Channel* channel;
    struct ChannelElement* next;
};

struct EventLoop
{
    bool isQuit;
    struct Dispatcher* dispatcher;
    void* dispatcherData;
    // 任务队列
    struct ChannelElememt* head;
    struct ChannelElement* tail;
    // map
    struct ChannelMap* channelMap;
    // 线程信息
    pthread_t threadID;
    char threadName[32];
    pthread_mutex_t mutex;
    // 存储两个本地通信的fd，当子线程阻塞，主线程要添加任务时，通过此fd唤醒子线程
    int socketPair[2];
};

// 初始化
struct EventLoop* eventLoopInit();
struct EventLoop* eventLoopInitEx(const char* threadName);

// 启动反应堆模型
int eventLoopRun(struct EventLoop* evLoop);

// 处理激活的文件描述符
int eventActivate(struct EventLoop* evLoop, int fd, int event);

// 添加任务到任务队列
int eventLoopAddTask(struct EventLoop* evLoop, struct Channel* channel, int type);

// 处理任务队列中的任务
int eventLoopProcessTask(struct EventLoop* evLoop);

// 处理dispatcher中的节点
int eventLoopAdd(struct EventLoop* evLoop, struct Channel* channel);
int eventLoopRemove(struct EventLoop* evLoop, struct Channel* channel);
int eventLoopModify(struct EventLoop* evLoop, struct Channel* channel);
// 释放channel
int destroyChannel(struct EventLoop* evLoop, struct Channel* channel);