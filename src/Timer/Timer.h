#ifndef TIMER_H
#define TIMER_H

#include <functional>
#include "common.h"
#include "TimeStamp.h"
class Timer
{
public:
    DISALLOW_COPY_AND_MOVE(Timer);
    Timer(TimeStamp timestamp, std::function<void()>const &cb, double interval,TimeUnit unit = TimeUnit::SECONDS);

    void ReStart(TimeStamp now);

    void run() const;
    TimeStamp expiration() const;
    bool repeat() const;

private:
    TimeStamp expiration_; // 定时器的绝对时间
    std::function<void()> callback_; // 到达时间后进行回调
    double interval_; // 如果重复，则重复间隔
    bool repeat_;
    TimeUnit unit_; // 时间单位
};
#endif // TIMER_H