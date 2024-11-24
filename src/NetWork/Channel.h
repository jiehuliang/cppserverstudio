
#ifndef CHANNEL_H
#define CHANNEL_H
#include <functional>
#include <sys/epoll.h>

class Epoll;
class EventLoop;

class Channel {
private:
  EventLoop *loop;
  int fd;
  uint32_t events;
  uint32_t revents;
  bool inEpoll;
  std::function<void()> callback;

public:
  Channel(EventLoop *loop, int _fd);
  ~Channel();

  void handleEvent();
  void enableReading();

  int getFd();

  uint32_t getEvents();
  uint32_t getRevents();
  bool getInEpoll();
  void setInEpoll();

  // void setEvents(uint32_t);
  void setRevents(uint32_t);
  void setCallback(std::function<void()>);
};

#endif // CHANNEL_H