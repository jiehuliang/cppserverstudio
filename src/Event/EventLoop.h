#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <functional>

class Epoll;
class Channel;
class ThreadPool;

class EventLoop {
private:
  Epoll *epoll;
  ThreadPool *threadPool;
  bool quit;

public:
  EventLoop();
  ~EventLoop();

  void loop();
  void updateChannel(Channel *);

  void addThread(std::function<void()>);
};

#endif // EVENTLOOP_H