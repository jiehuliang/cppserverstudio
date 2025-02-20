#include "Timer.h"
#include <functional>
#include "TimeStamp.h"
Timer::Timer(TimeStamp timestamp, std::function<bool()>const &cb, double interval, TimeUnit unit)
    : expiration_(timestamp),
      callback_(std::move(cb)),
      interval_(interval),
      repeat_(interval > 0.0),
      unit_(unit){};
    
void Timer::ReStart(TimeStamp now){
    expiration_ = TimeStamp::AddTime(now, interval_,unit_);
}


void Timer::run() {
    if (!callback_() || !(interval_ > 0.0)) {
        repeat_ = false;
    }
}

TimeStamp Timer::expiration() const { return expiration_; }

bool Timer::repeat() const { return repeat_; }


