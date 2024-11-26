#include "EventLoop.h"
#include <vector>
#include <memory>
#include "Channel.h"
#include "Epoller.h"

EventLoop::EventLoop() { poller_ = std::unique_ptr<Epoller>(new Epoller()); }

EventLoop::~EventLoop() {}

void EventLoop::Loop() const {
    while(true){
        for (Channel *active_ch : poller_->Poll()){
            active_ch->HandleEvent();
        }
    }
}

void EventLoop::UpdateChannel(Channel *ch) const { poller_->UpdateChannel(ch); }
void EventLoop::DeleteChannel(Channel *ch) const { poller_->DeleteChannel(ch); }