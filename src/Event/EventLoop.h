#ifndef EVENTLOOP_H
#define EVENTLOOP_H


class Epoll;
class Channel;

class EventLoop {
private:
	Epoll* epoll;
	bool quit;

public:
	EventLoop();
	~EventLoop();

	void loop();
	void updateChannel(Channel*);
};



#endif //EVENTLOOP_H