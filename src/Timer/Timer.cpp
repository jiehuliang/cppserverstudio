#include "Timer.h"
#include <functional>
#include "TimeStamp.h"
Timer::Timer(TimeStamp timestamp, std::function<void()>const &cb, double interval, TimeUnit unit)
    : expiration_(timestamp),
      callback_(std::move(cb)),
      interval_(interval),
      repeat_(interval > 0.0),
      unit_(unit){};
    
void Timer::ReStart(TimeStamp now){
    expiration_ = TimeStamp::AddTime(now, interval_,unit_);
}


void Timer::run() const{
    callback_();
}

void Timer::cancel() {
    callback_ = []() {};
    interval_ = 0.0;
}

TimeStamp Timer::expiration() const { return expiration_; }

bool Timer::repeat() const { return repeat_; }


