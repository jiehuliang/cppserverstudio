#include "Epoll.h"
#include "Channel.h"
#include "util.h"

#include <string.h>
#include <unistd.h>

#define MAX_EVENTS 1000

Epoll::Epoll() : epoll_fd(-1), events(nullptr) {
  epoll_fd = epoll_create1(0);
  errif(epoll_fd == -1, "epoll create error");
  events = new epoll_event[MAX_EVENTS];
  memset(events, 0, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll() {
  if (epoll_fd != -1) {
    close(epoll_fd);
    epoll_fd = -1;
  }
  delete[] events;
}

void Epoll::add(int fd, uint32_t op) {
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.data.fd = fd;
  ev.events = op;
  errif(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1,
        "epoll add event error");
}

std::vector<Channel *> Epoll::poll(int timeout) {
  std::vector<Channel *> activeChannels;
  int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
  errif(nfds == -1, "epoll wait error");
  for (int i = 0; i < nfds; ++i) {
    Channel *channel = (Channel *)events[i].data.ptr;
    channel->setRevents(events[i].events);
    activeChannels.push_back(channel);
  }
  return activeChannels;
}

void Epoll::updateChannel(Channel *channel) {
  int fd = channel->getFd();
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.data.ptr = channel;
  ev.events = channel->getEvents();
  if (!channel->getInEpoll()) {
    errif(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1,
          "epoll add event error");
    channel->setInEpoll();
  } else {
    errif(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1,
          "epoll mod event error");
  }
}

// std::vector<epoll_event> Epoll::poll(int timeout) {
//     std::vector<epoll_event> activeEvents;
//     int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
//     errif(nfds == -1,"epoll wait error");
//     for (int i = 0; i < nfds; ++i) {
//         activeEvents.push_back(events[i]);
//     }
//     return activeEvents;
// }
