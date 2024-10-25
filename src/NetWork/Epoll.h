#ifndef EPOLL_H
#define EPOLL_H
#include <sys/epoll.h>
#include <vector>

class Channel;


class Epoll {
public:
    Epoll();
    ~Epoll();
    void add(int fd, uint32_t op);
    void updateChannel(Channel*);
    std::vector<Channel*> poll(int timeout = -1);
    //std::vector<epoll_event> poll(int timeout = -1);
private:
    int epoll_fd;
    struct epoll_event *events;
};
#endif //EPOLL_H