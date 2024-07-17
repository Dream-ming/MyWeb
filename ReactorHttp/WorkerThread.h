#pragma once
#include <pthread.h>
#include "EventLoop.h"

// 定义子线程对应的结构体
struct WorkerThread
{
    pthread_t threadID;
    char name[24];
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    struct EventLoop* evLoop;
};

// 初始化
int workerThreadInit(struct WorkerThread* thread, int index);

// 启动线程
void workerThreadRun(struct WorkerThread* thread);