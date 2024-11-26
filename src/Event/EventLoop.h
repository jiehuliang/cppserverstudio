#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Epoller.h"
#include "common.h"

class Epoller;
class EventLoop
{
public:
    DISALLOW_COPY_AND_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();

    void Loop() const;
    void UpdateChannel(Channel *ch) const;
    void DeleteChannel(Channel *ch) const;

private:
    std::unique_ptr<Epoller> poller_;
};

#endif // EVENTLOOP_H
