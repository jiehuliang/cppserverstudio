#ifndef LOGGING_H
#define LOGGING_H

#include <string.h>
#include "common.h"
#include "TimeStamp.h"
#include "LogStream.h"

class Logger {
public:
	DISALLOW_COPY_AND_MOVE(Logger);
	enum LogLevel 
	{
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL
	};

	//编译器计算源文件名
	class SourceFile {
	public:
		SourceFile(const char* data);
		const char* data_;
		int size_;
	};

	//构造函数，主要是用于构造Impl
	Logger(const char* file_, int line, LogLevel level);
	~Logger();

	//用于日志宏，返回Impl的输出流
	LogStream& stream();

	//全局方法，设置日志全局日志级别，flush输出目的地
	static LogLevel logLevel();
	static void setLogLevel(LogLevel level);

	using OutputFunc = void(*)(const char* data, int  len);// 定义函数指针
	using FlushFunc = void(*)();
	//默认fwrite到stdout
	static void setOutput(OutputFunc);
	//默认flush到stdout
	static void setFlush(FlushFunc);

private:
	class Impl {
	public:
		DISALLOW_COPY_AND_MOVE(Impl);
		using LogLevel = Logger::LogLevel;
		Impl(Logger::LogLevel level, const SourceFile& file, int line);
		void FormattedTime();//格式化时间信息
		void Finish();// 完成格式化，并补充输出源码文件和源码位置

		LogStream& stream();
		const char* loglevel() const;//获取LogLevel的字符串
		LogLevel level_;
	private:
		Logger::SourceFile sourcefile_;//源代码名称
		int line_;//源代码行数
		LogStream stream_;//日志缓存流 
	};
	Impl impl;
};

//全局的日志级别，静态成员函数定义，静态成员函数实现
extern Logger::LogLevel g_logLevel;
inline Logger::LogLevel Logger::logLevel() {
	return g_logLevel;
}

//日志宏
#define LOG_DEBUG if (Logger::DEBUG <= Logger::DEBUG) \
	Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO if (Logger::logLevel() <= Logger::INFO) \
	Logger(__FILE__, __LINE__, Logger::INFO).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()

#endif //LOGGING_H