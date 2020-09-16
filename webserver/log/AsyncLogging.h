#pragma once

#include"../countdownlatch.h"
#include"../mutexlock.h"
#include"../thread/thread.h"
#include"logstream.h"
#include"../noncopyable.h"
#include<functional>
#include<string>
#include<vector>
class AsyncLogging:noncopyable{
public:

	AsyncLogging(const std::string basename, int flush=2);
	~AsyncLogging();
	void append(const char* logline,int len);
	void start();
	void stop();

private:

	AsyncLogging(const AsyncLogging &a);
	void operator=(const AsyncLogging& b);	
	
	void threadFunc();
		
	typedef FixedBuffer<KLargeBuffer> Buffer;
	typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
	typedef std::shared_ptr<Buffer> BufferPtr

	const int flushCount_;
	bool running_;
	std::string basename_;
	Thread thread_;
	MutexLock mutex_;
	Conition cond_;
	BufferPtr currentBuffer_;
	BufferPtr nextBuffer_;
	CountDownLatch latch_;	
};
