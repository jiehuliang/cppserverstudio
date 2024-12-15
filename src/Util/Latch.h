#ifndef LATCH_H
#define LATCH_H

#include "common.h"
#include <thread>
#include <condition_variable>
class Latch {
private:
	std::mutex mutex_;
	std::condition_variable conv_;
	int count_;

public:
	DISALLOW_COPY_AND_MOVE(Latch);

	explicit Latch(int count) : count_(count) {}
	void wait() {
		std::unique_lock<std::mutex> lock(mutex_);
		while (count_ > 0) {
			conv_.wait(lock);
		}
	}

	void notify() {
		std::unique_lock<std::mutex> lock(mutex_);
		--count_;
		if (count_ == 0) {
			conv_.notify_all();
		}
	}

};

#endif // LATCH_H