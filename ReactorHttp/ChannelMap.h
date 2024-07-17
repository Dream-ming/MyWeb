#pragma once
#include <stdbool.h>

struct ChannelMap
{
    // ��¼ָ��ָ��������Ԫ���ܸ���
    int size;
    // ����ָ�룬ָ��һ��Channel���͵�ָ������
    struct Channel** list;
};

// ��ʼ��
struct ChannelMap* channelMapInit(int size);

// ���map
void ChannelMapClear(struct ChannelMap* map);

// ���ݺ���
bool makeMapRoom(struct ChannelMap* map, int newSize, int unitSize);