#include "WorkerThread.h"

int workerThreadInit(struct WorkerThread* thread, int index)
{
    thread->evLoop = NULL;
    thread->threadID = 0;
    sprintf(thread->name, "SubThread-%d", index);
    pthread_mutex_init(&thread->mutex, NULL);
    pthread_cond_init(&thread->cond, NULL);
    return 0;
}

// 子线程的回调函数
void* subThreadRunning(void* arg)
{
    struct WorkerThread* thread = (struct WorkerThread*)arg;
    thread->evLoop = eventLoopInitEx(thread->name);
    eventLoopRun(thread->evLoop);
    return NULL;
}

void workerThreadRun(struct WorkerThread* thread)
{
    // 创建子线程
    pthread_create(&thread->threadID, NULL, subThreadRunning, thread);
    // 阻塞主线程，确保子线程成功创建反应堆模型
    pthread_mutex_lock(&thread->mutex);
    while (thread->evLoop == NULL)
    {
        pthread_cond_wait(&thread->cond, &thread->mutex);
    }
}


