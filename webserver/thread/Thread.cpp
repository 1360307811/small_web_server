#include "Thread.h"
#include "CurrentThread.h"
//#include "Logging.h"
#include "CountDownLatch.h"
#include <memory>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include<pthread.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <assert.h>

#include <iostream>
using namespace std;
namespace CurrentThread{
	__thread int t_cachedTid;/*线程id*/
	__thread char t_tidString[32];/*线程id的string 形式*/
	__thread int t_tidStringLength;/*线程id的string 形式的长度 */
	__thread const char * t_threadName;/*线程名*/

	/*获取线程的id string_id length*/
	void cacheTid();
	/*   将流水线引入cpu，可以提高cpu的效率。更简单的说，让cpu可以预先取出下一条指令，可以提供cpu的效率
 *   	绝大部分情况下，t_cachedTid==0为假，函数cacheTid()在绝大部分情况下不执行，完成了对代码的技能预判。
 *      cpu提前装载cacheTid()的指令，以达到优化和效率的目的。 */
	int inline tid(){
		if(__builtin_expect(t_cachedTid==0,0)){
			cacheTid();
		}
		return t_cachedTid;
	}
	  inline const char* tidString(){
        	return t_tidString;
    	}

    	inline int tidStringLength(){
        	return t_tidStringLength;
   	 }

    	inline const char* name(){
        	return t_threadName;
    	}
	
}//namesapce CurrentThread
	
pid_t gettid(){

	return static_cast<pid_t>(::syscall(SYS_gettid));
}
void CurrentThread::cacheTid(){
		if(t_cachedTid==0){
			t_cachedTid= gettid();
			t_tidStringLength=snprintf(t_tidString,sizeof(t_tidString),"%5d",t_cachedTid);
		}

	}


struct ThreadData{
	typedef Thread::ThreadFunc ThreadFunc;
	ThreadFunc func_;
	string name_;
	pid_t* tid_;
	CountDownLatch* latch_;
	ThreadData(const ThreadFunc& func,const string& name,pid_t* tid,CountDownLatch* latch)
        : func_(func),name_(name),tid_(tid),latch_(latch){ }
	
	void runInThraed(){
		cout<<"runInThraed......"<<endl;
		*tid_=CurrentThread::tid();
		tid_=NULL;
		latch_->countDown();
		latch_=NULL;
			
		CurrentThread::t_threadName=name_.empty()?"Thread":name_.c_str();
		prctl(PR_SET_NAME,CurrentThread::t_threadName);/*这个系统调用指令是为进程制定而设计的，明确的选择取决于option: PR_SET_NAME :把参数arg2作为调用进程的经常名字。（SinceLinux 2.6.11）*/
		func_();
		CurrentThread::t_threadName="finish";
	}

};
/*线程run的函数*/
void *startThread(void * arg ){
	cout<<"startThread........"<<endl;
	ThreadData* data=static_cast<ThreadData*>(arg);
	data->runInThraed();
	delete data;
	return NULL;

}

Thread::Thread(const ThreadFunc& func, const string& n)
  : started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(0),
    func_(func),
    name_(n),
    latch_(1)
{
cout<<"Thread............"<<endl;
  setDefaultName();
}

Thread::~Thread()
{
  if (started_ && !joined_)
  {
    pthread_detach(pthreadId_);
  }
}
void Thread::setDefaultName(){
	cout<<"setDefaultName.........."<<endl;
	if(name_.empty()){
		char buf[32];
		snprintf(buf,sizeof(buf),"Thread");
		name_=buf;
	}


}

void Thread::start(){
	cout<<"start..........."<<endl;
	assert(!started_);
	started_=true;
	ThreadData* data=new ThreadData(func_,name_,&tid_,&latch_);
	if(pthread_create(&pthreadId_ ,NULL,&startThread,data)){
		started_=false;
		delete data;
	}else{
		latch_.wait();
		assert(tid_>0);
	}
}
int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}


