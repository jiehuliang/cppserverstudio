#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "ThreadPool.h"
#include <vector>

EventLoop::EventLoop():epoll(nullptr), threadPool(nullptr),quit(false) {
	epoll = new Epoll();
	threadPool = new ThreadPool();
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

void EventLoop::addThread(std::function<void()> func) {
	threadPool->add(func);
}

