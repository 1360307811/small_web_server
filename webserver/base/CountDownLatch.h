#pragam once
#include"noncopyable.h"
#include"mutex.h"
#include"condition"
/*倒计时：*/
class CountDownLath:noncopyable{
public:


	explicit CountDownLath(int count):mutex_(),condition_(mutex_),count_(count){}
	void wait();/*等待主线程的起跑号令*/
	void countDown();/*主线程通知各个线程起跑*/	
	void getCount()const{ 
		MutexLockGuard lock(mutex_);
		return count_;}

private:
	mutable MutexLock mutex_;
	Condition condition_;
	int count_;
};

 
