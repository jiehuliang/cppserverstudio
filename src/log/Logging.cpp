#include "Logging.h"
#include "CurrentThread.h"

//为了实现多线程中日志时间格式化的效率，增加了两个_thread变量，
//用于缓存当前线程存日期时间字符串、上一次日志记录的秒数
_thread char t_time[64];		//当前线程存日期的时间字符串 “年:月:日 时:分:秒”
_thread time_t t_lastsecond;	//当前线程上一次记录日志的秒数

//方便一个已知长度的字符串被送入buffer中
class Template {
public:
	Template(const char**  str, unsigned int len)
		: str_(str),
		  len_(len) {}
	const char* str_;
	const unsigned int len_;
};

