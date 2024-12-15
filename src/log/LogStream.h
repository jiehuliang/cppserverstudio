#ifndef LOG_STREAM_H
#define LOG_STREAM_H

#include "common.h"
#include <string>
#include <algorithm>
#include <cstring>
#include <assert.h>
#include <iostream>

static const int FixedBufferSize = 4096;
static const int FixedLargeBufferSize = 4096 * 1000;
static const int kMaxNumericSize = 48;
template <int SIZE>
class FixedBuffer {
	//修改成模板类，方便后端和前端的不同Buffer大小
public:
	FixedBuffer();
	~FixedBuffer();

	void append(const char* buf,int len);
	const char* data() const;
	int len() const;

	char* current();
	int avail() const; //剩余的可用空间
	void add(int len);

	void reset();//重置缓冲区
	const char* end() const;//获取末端地址
	void bzero();
private:
	char data_[SIZE];
	char* cur_;
};

class LogStream {
public:
	DISALLOW_COPY_AND_MOVE(LogStream);
	using self = LogStream;
	using Buffer = FixedBuffer<FixedBufferSize>;

	LogStream();
	~LogStream();

	void append(const char* data, int len);
	const Buffer& buffer() const;
	void resetBuffer();

	self &operator<<(bool v);
	//整形数据的字符转换、保存到缓冲区；内部均调用void formatInteger(T val);函数
	self& operator<<(short num);
	self& operator<<(unsigned short num);
	self& operator<<(int num);
	self& operator<<(unsigned int num);
	self& operator<<(long num);
    self& operator<<(unsigned long num);
	self& operator<<(long long num);
	self& operator<<(unsigned long long num);

	//浮点数数据转换、保存到缓冲区
	self& operator<<(const float& num);
	self& operator<<(const double& num);

	self& operator<<(char v);
	//原生字符串输出到缓冲区
	self& operator<<(const char* str);

	//标准字符串std::string输出到缓冲区
	self& operator<<(const std::string& v);
private:
	template <typename T>
	void formatInteger(T value);

	Buffer buffer_;
};

template <typename T>
void LogStream::formatInteger(T value) {
	if (buffer_.avail() >= kMaxNumericSize) {
		char* buf = buffer_.current();
		char* now = buf;

		do {
			int remainder = value % 10;
			*(now++) = remainder + '0'; //加上字符'0'的ASCII值来得到对应数字的字符表示
			value /= 10;
		} while (value != 0);
		if (value < 0) {
			*(now++) = '-';
		}
		std::reverse(buf, now);//使用std::reverse(buf, now)函数反转逆序的字符序列，使其变为正序。
		buffer_.add(now - buf);
	}
}

class Fmt {
public:
	template<typename T>
	Fmt(const char* fmt, T val);

	const char* data() const { return buf_; }
	
	int length() const { return length_; }
private:
	char buf_[32];
	int length_;
};

template<typename T>
Fmt::Fmt(const char* fmt,T val) {
	static_assert(std::is_arithmetic<T>::value == true, "Must be arithmetic type");
	
	length_ = snprintf(buf_,sizeof(buf_),fmt,val);
	assert(static_cast<size_t>(length_) < sizeof(buf_));
}

inline LogStream& operator<<(LogStream& s,const Fmt& fmt) {
	s.append(fmt.data(),fmt.length());
	return s;
}

// Explicit instantiations

template Fmt::Fmt(const char* fmt, char);

template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);


template<int SIZE>
FixedBuffer<SIZE>::FixedBuffer() :cur_(data_) {};

template<int SIZE>
FixedBuffer<SIZE>::~FixedBuffer() {};

template<int SIZE>
void FixedBuffer<SIZE>::append(const char* buf, int len) {
	if (avail() > len) {
		memcpy(cur_, buf, len);
		cur_ += len;
	}
}

template<int SIZE>
const char* FixedBuffer<SIZE>::data() const { return data_; }

template<int SIZE>
int FixedBuffer<SIZE>::len() const { return static_cast<int>(cur_ - data_); }

template<int SIZE>
char* FixedBuffer<SIZE>::current() { return cur_; }

template<int SIZE>
int FixedBuffer<SIZE>::avail() const { return static_cast<int>(end() - cur_); }

template<int SIZE>
void FixedBuffer<SIZE>::add(int len) { cur_ += len; }

template<int SIZE>
void FixedBuffer<SIZE>::reset() { cur_ = data_; }

template<int SIZE>
const char* FixedBuffer<SIZE>::end() const { return data_ + sizeof(data_); }

template<int SIZE>
void FixedBuffer<SIZE>::bzero() {
    memset(data_, '\0', sizeof(data_));
    cur_ = data_;
}




#endif // LOG_STREAM_H