#pragma once
#include <stdbool.h>

// ���庯��ָ��
typedef int(*handleFunc)(void* arg);

// �����ļ��������Ķ�д�¼�
enum FDEvent
{
    TimeOut = 0x01,
    ReadEvent = 0x02,
    WriteEvent = 0x04
};

struct Channel
{
    // �ļ�������
    int fd;
    // �¼�
    int events;
    // �ص�����
    handleFunc readCallback;
    handleFunc writeCallback;
    // �ص������Ĳ���
    void* arg;
};

// ��ʼ��һ��Channel
struct Channel* channelInit(int fd, int events, handleFunc readFunc, handleFunc writeFunc, void* arg);

// �޸�fd��д�¼�(��� or �����)
void writeEventEnable(struct Channel* channel, bool flag);

// �ж��Ƿ���Ҫ�޸ļ���ļ���������д�¼�
bool isWriteEventEnable(struct Channel* channel);