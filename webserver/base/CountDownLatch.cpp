#include"CountDownLatch.h"
void CountDownLatch::wait(){
	MutexLockGuard lock(mutex_);
	while(count_>0){
		condition_.wait();
	}
}
void CountDownLatch::countDown(){
	MutexLockGuard lock(mutex_);
	--count_;
	if(count_==0){
		condition_.notifyAll();
	}
}



