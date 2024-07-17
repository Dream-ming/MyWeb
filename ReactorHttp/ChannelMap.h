#pragma once
#include <stdbool.h>

struct ChannelMap
{
    // 记录指针指向的数组的元素总个数
    int size;
    // 二级指针，指向一个Channel类型的指针数组
    struct Channel** list;
};

// 初始化
struct ChannelMap* channelMapInit(int size);

// 清空map
void ChannelMapClear(struct ChannelMap* map);

// 扩容函数
bool makeMapRoom(struct ChannelMap* map, int newSize, int unitSize);