#include "AsyncLogging.h"
#include  "LogFile.h"

#include <vector>
#include <memory>
#include <functional>
#include <chrono>

AsyncLogging::AsyncLogging(const char* filepath) 
	:running_(false),
	filepath_(filepath),
	latch_(1){
	current_ = std::unique_ptr<Buffer>(new Buffer());
	next_ = std::unique_ptr<Buffer>(new Buffer());
}

AsyncLogging::~AsyncLogging() {
	if (running_) {
		Stop();
	}
}

void AsyncLogging::Start() {
	running_ = true;
	thread_ = std::thread(std::bind(&AsyncLogging::ThreadFunc, this));

	//等待线程启动完成
	latch_.wait();
}

void AsyncLogging::Stop() {
	running_ = false;
	//唤醒后端线程
	conv_.notify_one();
	thread_.join();
}

void AsyncLogging::Flush() {
	fflush(stdout);
}

void AsyncLogging::Append(const char* data, int len) {
	std::unique_lock<std::mutex> lock(mutex_);
	if (current_->avail() >= len) {
		current_->append(data, len);
	}
	else{
		// 如果当前缓存没有空间，就将当前缓存放入到已满列表中
		buffers_.push_back(std::move(current_));
		if (next_) {
			current_ = std::move(next_);
		}
		else {
			current_.reset(new Buffer());
		}
		//向新的缓冲区写入信息。
		current_->append(data, len);
	}
	// 唤醒后端线程
	conv_.notify_one();
}

void AsyncLogging::ThreadFunc() {
	//创建成功，提醒主线程
	latch_.notify();

	std::unique_ptr<Buffer> new_current = std::unique_ptr<Buffer>(new Buffer());
	std::unique_ptr<Buffer> new_next = std::unique_ptr<Buffer>(new Buffer());

	std::unique_ptr<LogFile> logfile = std::unique_ptr<LogFile>(new LogFile());

	new_current->bzero();
	new_next->bzero();

	std::vector<std::unique_ptr<Buffer>> active_buffers;

	while (running_) {
		std::unique_lock<std::mutex> lock(mutex_);
		if (buffers_.empty()) {
			//如果队列中还没有已满的缓冲区，则等待片刻
			conv_.wait_until(lock, std::chrono::system_clock::now() +
				BufferWriteTimeout * std::chrono::milliseconds(1000),
				[] {return false; });
		}

		//直接将当前缓冲区加入到已满缓冲区中，
		buffers_.push_back(std::move(current_));
		active_buffers.swap(buffers_);

		current_ = std::move(new_current);

		if (!next_) {
			next_ = std::move(new_next);
		}

		//写入日志文件
		for (const auto& buffer : active_buffers) {
			logfile->Write(buffer->data(), buffer->len());
		}

		if (logfile->writtenbytes() >= FileMaximumSize) {
			//指向一个新的日志文件
			logfile.reset(new LogFile(filepath_));
		}

		if (active_buffers.size() > 2) {
			//留住两个，用于后续
			active_buffers.resize(2);
		}

		if (!new_current) {
			new_current = std::move(active_buffers.back());
			active_buffers.pop_back();
			new_current->bzero();
		}
		if (!new_next) {
			new_next = std::move(active_buffers.back());
			active_buffers.pop_back();
			new_next->bzero();
		}

		active_buffers.clear();
	}
}