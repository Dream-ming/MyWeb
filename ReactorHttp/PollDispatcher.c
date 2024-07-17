#include "Dispatcher.h"
#include <sys/poll.h>

#define Max 1024
struct PollData
{
    int maxfd;
    struct pollfd fds[Max];
};

static void* pollInit();
static int pollAdd(struct Channel* channel, struct EventLoop* evLoop);
static int pollRemove(struct Channel* channel, struct EventLoop* evLoop);
static int pollModify(struct Channel* channel, struct EventLoop* evLoop);
static int pollDispatch(struct EventLoop* evLoop, int timeout); // 单位：s
static int pollClear(struct EventLoop* evLoop);

struct Dispatcher PollDispatcher = {
    pollInit,
    pollAdd,
    pollRemove,
    pollModify,
    pollDispatch,
    pollClear
};

static void* pollInit()
{
    struct PollData* data = (struct PollData*)malloc(sizeof(struct PollData));
    data->maxfd = 0;
    for (int i = 0; i < Max; ++i)
    {
        data->fds[i].fd = -1;
        data->fds[i].events = 0;
        data->fds[i].revents = 0;
    }

    return data;
}

static int pollAdd(struct Channel* channel, struct EventLoop* evLoop)
{
    struct PollData* data = (struct PollData*)evLoop->dispatcher;
    int events = 0;
    if (channel->events & ReadEvent)
    {
        events |= POLLIN;
    }
    if (channel->events & WriteEvent)
    {
        events |= POLLOUT;
    }
    int i = 0;
    for (i = 0; i < Max; ++i)
    {
        if (data->fds[i].fd == -1)
        {
            data->fds[i].events = events;
            data->fds[i].fd = channel->fd;
            data->maxfd = data->maxfd > i ? data->maxfd : i;
            break;
        }
    }
    if (i >= Max)
    {
        return -1;
    }
    return 0;
}

static int pollRemove(struct Channel* channel, struct EventLoop* evLoop)
{
    struct PollData* data = (struct PollData*)evLoop->dispatcher;
    int i = 0;
    for (i = 0; i < Max; ++i)
    {
        if (data->fds[i].fd == channel->fd)
        {
            data->fds[i].events = 0;
            data->fds[i].revents = 0;
            data->fds[i].fd = -1;
            break;
        }
    }
    if (i >= Max)
    {
        return -1;
    }
    return 0;
}

static int pollModify(struct Channel* channel, struct EventLoop* evLoop)
{
    struct PollData* data = (struct PollData*)evLoop->dispatcher;
    int events = 0;
    if (channel->events & ReadEvent)
    {
        events |= POLLIN;
    }
    if (channel->events & WriteEvent)
    {
        events |= POLLOUT;
    }
    int i = 0;
    for (i = 0; i < Max; ++i)
    {
        if (data->fds[i].fd == channel->fd)
        {
            data->fds[i].events = events;
            break;
        }
    }
    if (i >= Max)
    {
        return -1;
    }
    return 0;
}

static int pollDispatch(struct EventLoop* evLoop, int timeout) // 单位：s
{
    struct PollData* data = (struct PollData*)evLoop->dispatcherData;
    int count = poll(data->fds, data->maxfd + 1, timeout * 1000);
    if (count == -1)
    {
        perror("poll");
        exit(0);
    }
    for (int i = 0; i <= data->maxfd; ++i)
    {
        if (data->fds[i].fd == -1)
        {
            continue;
        }
        if (data->fds[i].revents & POLLIN)
        {
            eventActivate(evLoop, data->fds[i].fd, ReadEvent);
        }
        if (data->fds[i].revents & POLLOUT)
        {
            eventActivate(evLoop, data->fds[i].fd, WriteEvent);
        }
    }
    return 0;
}

static int pollClear(struct EventLoop* evLoop)
{
    struct PollData* data = (struct PollData*)evLoop->dispatcherData;
    free(data);
}