#ifndef  _CONDITION_H_
#define  _CONDITION_H_
#include<pthread.h>
#include"mutex.h"
#include "noncopyable.h"
#include <errno.h>
#include <cstdint>
#include <time.h>
class Condition:noncopyable{
public:
	explicit Condition(MutexLock& mutex):mutex_(mutex){
		pthread_cond_init(&cond_,NULL);
	}
	~ Condition(){
		pthread_cond_destroy(&cond_);
	}

	void wait(){
		//MutexLocku::UnassignGuard ug(mutex_);
		pthread_cond_wait(&cond_,mutex_.getPthread_Mutex());
	}
	//限制时间的等待
	bool waitForSendonds(double seconds);

	void notify(){
		pthread_cond_signal(&cond_);
	}
	void notifyAll(){
		pthread_cond_broadcast(&cond_);
	}
	bool waitForSeconds(int seconds){
		struct timespec abstime;
		clock_gettime(CLOCK_REALTIME,&abstime);
		abstime.tv_sec+=static_cast<time_t>(seconds);	
		return ETIMEDOUT==pthread_cond_timedwait(&cond_,mutex_.getPthread_Mutex(),&abstime);
	}
private:
	MutexLock& mutex_;
	pthread_cond_t cond_;
};
#endif 
