#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>

class ThreadPool {
private:
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;
	std::mutex tasks_mtx;
	std::condition_variable cv;
	bool stop;
public:
	ThreadPool(int size = 10);
	~ThreadPool();

	void add(std::function<void()>);
};

#endif //THREAD_POOL_H