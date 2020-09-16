#pragma once

#include"fileutil.h"
#include"../mutex.h"
#include"../noncopyable.h"
#include<memory>
#include<string>

class LogFile:noncopyable{
public:
	LogFile(const string& basename,int flush_count);/*构造最底层 AppendFile*/
	~LogFile();
	/*调用本类的 append_unlocked*/
	void append(const char* logline,int line);/*表层写日志*/
	/*调用fileutil 的flush**/
	void flush();/*调用AppendFile ->flush*/
	
	bool rollFile();
private:
	/*调用fileutil的apeend  并在条件达到时 调用fileutil的flush*/
	void append_unlocked(const char* logline ,int len);/*控制AppendFile的写入和flush*/
	
	const std::string basename_;
	const int flush_;
	int count_;
	std::unique_ptr<MutexLock> mutex_;
	/*fileutil 的指针*/
	std::unique_ptr<AppendFile> file_;
};
