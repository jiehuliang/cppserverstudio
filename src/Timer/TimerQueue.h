#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include <set>
#include <vector>
#include <memory>
#include <functional>
#include "TimeStamp.h"
#include "common.h"

class EventLoop;
class Channel;
class Timer;
class TimerQueue {
public:
	DISALLOW_COPY_AND_MOVE(TimerQueue);
	TimerQueue(EventLoop* loop);
	~TimerQueue();

	void CreateTimerfd(); //创建timerfd
	void HandleRead(); //timerfd可读时调用

	void ResetTimerFd(Timer* timer); //重新设置timerfd超时时间，关注新的定时任务
	void ResetTimers(); //将具有重复属性的定时器重新加入队列

	bool Insert(Timer* timer); //将定时任务插入队列
	void AddTimer(TimeStamp timestamp, std::function<void()>const& cb, double interval); //添加一个定时任务

private:
	using  Entry =  std::pair<TimeStamp, Timer*>;

	EventLoop* loop_;
	int timerfd_:
	std::unique_ptr<Channel> channel_;

	std::set<Entry> timers_; //定时器集合
	std::vector<Entry> active_timers_; //激活的定时器
};

#endif // TIMERQUEUE_H