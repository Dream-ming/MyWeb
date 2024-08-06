#pragma once
#include "Channel.h"
#include "EventLoop.h"
#include "Dispatcher.h"
#include <string>
#include <poll.h>
using namespace std;

class PollDispatcher : public Dispatcher
{
public:
    PollDispatcher(EventLoop* evloop);
    ~PollDispatcher();
    // ���
    int add() override;
    // ɾ��
    int remove() override;
    // �޸�
    int modify() override;
    // �¼����
    int dispatch(int timeout = 2) override; // ��λ: s

private:
    int m_maxfd;
    struct pollfd* m_fds;
    const int m_maxNode = 1024;
};