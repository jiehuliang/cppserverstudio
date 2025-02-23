#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "common.h"
#include "Timer.h"

#include <memory>
#include <mutex>
#include <thread>
#include <functional>
#include <vector>
class Epoller;
class TimerQueue;
class TimeStamp;
class EventLoop
{
public:
    DISALLOW_COPY_AND_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();
    
    void Loop();
    void UpdateChannel(Channel *ch);
    void DeleteChannel(Channel *ch);

    //��ʱ������
    void RunAt(TimeStamp timestamp, std::function<bool()> const& cb);
    void RunAfter(double wait_time, std::function<bool()>const& cb, TimeUnit unit = TimeUnit::SECONDS);
    Timer::TimerPtr RunEvery(double interval,  std::function<bool()>const& cb, TimeUnit unit = TimeUnit::SECONDS);

    // ���ж����е�����
    void DoToDoList();

    // ���������ӵ������С���loop���polling������
    void QueueOneFunc(std::function<void()> fn); 

    // ����ɴ�����Loop���̵߳��ã�������ִ��fn����
    // ���򣬽�fn���뵽�����У��ȴ�֮������
    void RunOneFunc(std::function<void()> fn);
    
    // �жϵ��øú������ǲ��ǵ�ǰ���̣߳����ǲ��Ǵ�����ǰLoop���̡߳�
    bool IsInLoopThread();

    void HandleRead();

private:
    std::unique_ptr<Epoller> poller_;
    std::vector<std::function<void()>> to_do_list_;
    std::mutex mutex_;

    int wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channel_;

    bool calling_functors_;
    pid_t tid_;

    std::unique_ptr<TimerQueue> timer_queue_;
};

#endif // EVENTLOOP_H
