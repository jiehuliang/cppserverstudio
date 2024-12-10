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
	DISALLOW_MOVE_AND_COPY(Latch);

	explicit Latch(int count) : count_(count) {}
	void wait() {
		std::unique_lock<std::mutex> lock(mutex_);
		while (count > 0) {
			conv_.wait(lock);
		}
	}

	void notify() {
		std::unique_lock<std::mutex> lock(mutex_);
		--counnt_;
		if (count_ == 0) {
			conv_.notify_all();
		}
	}

};

#endif // LATCH_H