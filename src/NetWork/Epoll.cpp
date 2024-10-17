#include "Epoll.h"
#include "util.h"

#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1000

Epoll::Epoll():epoll_fd(-1),events(nullptr)
{
    epoll_fd = epoll_create1(0);
    errif(epoll_fd ==-1,"epoll create error");
    events = new epoll_event[MAX_EVENTS];
    memset(events,0,sizeof(events));
}

Epoll::~Epoll() {
    if(epoll_fd != -1){
        close(epoll_fd);
        epoll_fd = -1;
    }
    delete[] events;
}

void Epoll::add(int fd,uint32_t op) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1 , "epoll add event error");
}

std::vector<epoll_event> Epoll::poll(int timeout) {
    std::vector<epoll_event> activeEvents;
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
    errif(nfds == -1,"epoll wait error");
    for (int i = 0; i < nfds; ++i) {
        activeEvents.push_back(events[i]);
    }
    return activeEvents;
}


