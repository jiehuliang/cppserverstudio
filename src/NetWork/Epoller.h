#ifndef EPOLLER_H
#define EPOLLER_H
#include "common.h"

#include <vector>
#include <sys/epoll.h>

class Channel;
class Epoller
{
public:
    DISALLOW_COPY_AND_MOVE(Epoller);

    Epoller();
    ~Epoller();

    // ?????channel
    void UpdateChannel(Channel *ch) const;
    // ???????
    void DeleteChannel(Channel *ch) const;

    // ?????epoll_wait?????
    std::vector<Channel *> Poll(long timeout = -1) const;

    private:
        int fd_;
        struct epoll_event *events_;
};
#endif // EPOLLER_H
