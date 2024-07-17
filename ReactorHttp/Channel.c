#include "Channel.h"

struct Channel* channelInit(int fd, int events, handleFunc readFunc, handleFunc writeFunc, void* arg)
{
    struct Channel* channel = (struct Channel*)malloc(sizeof(struct Channel));
    channel->arg = arg;
    channel->fd = fd;
    channel->events = events;
    channel->readCallback = readFunc;
    channel->writeCallback = writeFunc;
    return channel;
}

void writeEventEnable(struct Channel* channel, bool flag)
{
    if (flag)
    {
        channel->events |= WriteEvent;
    }
    else
    {
        channel->events &= (~WriteEvent);
    }
}

bool isWriteEventEnable(struct Channel* channel)
{
    return channel->events & WriteEvent;
}
