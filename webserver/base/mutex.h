#ifndef _MUTEX_H_
#define _MUTEX_H_
#include<iostream>
#include<pthread.h>
#include<assert.h>
#include<error.h>
#include<boost/noncopyable.hpp>
//#include"/home/kiosk/桌面/muduo-master/muduo/base/CurrentThread.h"
//#include <boost/nocopyable.hpp>
using namespace std;
//锁的基础类

class MutexLock:private boost::noncopyable{
public:
	MutexLock():holder_(0){
		pthread_mutex_init(&mutex_,NULL);
	}

	~MutexLock(){
		assert(holder_==0);//表示此时没有线程被锁 所以就不用希构
		pthread_mutex_destroy(&mutex_);
	}
	/*
	//查看那当前线程是否被锁
	bool isLockedByThisThread()const{
		return holder_=CurrentThread::tid();
	}
*/
	//如果此时没有线程被锁 则编译中断
/*	void assertLocked(){
		assert(isLockedByThisThread());
	}
*/
	void lock(){
		pthread_mutex_lock(&mutex_);
		
	}

	void unlock(){
		holder_=0;
		pthread_mutex_unlock(&mutex_);
	}
	
	pthread_mutex_t* getPthread_Mutex(){
		return &mutex_;
	}
private:
	friend class Condition;//声明条件变量为mutex类的友元函数

//成员变量
	pthread_mutex_t mutex_;
	pid_t holder_;
};

//用来加锁 减锁。
class MutexLockGuard:boost::noncopyable{
public:
	explicit MutexLockGuard(MutexLock& mutex):mutex_(mutex){
		mutex_.lock();
	}
	~MutexLockGuard(){
		mutex_.unlock();
	}

private:

MutexLock& mutex_;

};


//#define MutexLockGuard(x) assert(false,"生成临时对象昂")
#endif
