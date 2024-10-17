#ifndef EPOLL_H
#define EPOLL_H
#include <sys/epoll.h>
#include <vector>


class Epoll {
public:
    Epoll();
    ~Epoll();
    void add(int fd, uint32_t op);
    void del(int fd) ;
    void update(int fd, int events);
    std::vector<epoll_event> poll(int timeout = -1);
private:
    int epoll_fd;
    struct epoll_event *events;
};
#endif