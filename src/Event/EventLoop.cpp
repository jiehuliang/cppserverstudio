#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include <vector>

EventLoop::EventLoop():epoll(nullptr),quit(false) {
	epoll = new Epoll();
}

EventLoop::~EventLoop() {
	delete epoll;
}

void EventLoop::loop() {
	while (!quit) {
		std::vector<Channel*> chs;
		chs = epoll->poll();
		for (auto it = chs.begin(); it != chs.end();++it) {
			(*it)->handleEvent();
		}
	}
}

void EventLoop::updateChannel(Channel* ch) {
	epoll->updateChannel(ch);
}

