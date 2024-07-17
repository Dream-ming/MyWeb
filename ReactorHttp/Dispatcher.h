#pragma once
#include "Channel.h"
#include "EventLoop.h"

struct Dispatcher
{
    // init -- ��ʼ��epoll��poll��select��Ҫ�����ݿ�
    void* (*init)();
    // ���
    int (*add)(struct Channel* channel, struct EventLoop* evLoop);
    // ɾ��
    int (*remove)(struct Channel* channel, struct EventLoop* evLoop);
    // �޸�
    int (*modify)(struct Channel* channel, struct EventLoop* evLoop);
    // �¼����
    int (*dispatch)(struct EventLoop* evLoop, int timeout); // ��λ��s
    // �������(�ر�fd���ͷ��ڴ�)
    int (*clear)(struct EventLoop* evLoop);
};