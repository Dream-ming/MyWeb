#pragma once
#include <stdbool.h>
#include <pthread.h>
#include "Dispatcher.h"
#include "ChannelMap.h"

// ���ļ���������
extern struct Dispatcher EpollDispatcher;
extern struct Dispatcher PollDispatcher;
extern struct Dispatcher SelectDispatcher;

// ����Ľڵ��е�channel�ķ�ʽ
enum ElemType{ADD, DELETE, MODIFY};
// ����������еĽڵ�
struct ChannelElement
{
    int type; // ����channel�ķ�ʽ
    struct Channel* channel;
    struct ChannelElement* next;
};

struct EventLoop
{
    bool isQuit;
    struct Dispatcher* dispatcher;
    void* dispatcherData;
    // �������
    struct ChannelElememt* head;
    struct ChannelElement* tail;
    // map
    struct ChannelMap* channelMap;
    // �߳���Ϣ
    pthread_t threadID;
    char threadName[32];
    pthread_mutex_t mutex;
    // �洢��������ͨ�ŵ�fd�������߳����������߳�Ҫ�������ʱ��ͨ����fd�������߳�
    int socketPair[2];
};

// ��ʼ��
struct EventLoop* eventLoopInit();
struct EventLoop* eventLoopInitEx(const char* threadName);

// ������Ӧ��ģ��
int eventLoopRun(struct EventLoop* evLoop);

// ��������ļ�������
int eventActivate(struct EventLoop* evLoop, int fd, int event);

// ��������������
int eventLoopAddTask(struct EventLoop* evLoop, struct Channel* channel, int type);

// ������������е�����
int eventLoopProcessTask(struct EventLoop* evLoop);

// ����dispatcher�еĽڵ�
int eventLoopAdd(struct EventLoop* evLoop, struct Channel* channel);
int eventLoopRemove(struct EventLoop* evLoop, struct Channel* channel);
int eventLoopModify(struct EventLoop* evLoop, struct Channel* channel);
// �ͷ�channel
int destroyChannel(struct EventLoop* evLoop, struct Channel* channel);