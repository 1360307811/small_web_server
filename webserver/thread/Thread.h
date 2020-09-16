#ifndef _THREAD_H__
#define _THREAD_H__
#include<pthread.h>
#include"CountDownLatch.h"
#include"../noncopyable.h"
#include<functional>
#include<memory>
#include<string>
class Thread:noncopyable{
public:
	typedef std::function<void ()>  ThreadFunc;
	explicit Thread(const ThreadFunc& func,const std:: string& str=std::string());
	//虚构函数一定的是虚函数
	 ~Thread();

	void start();
	int join();
	//函数功能 ：当线程结束时自动销毁对象
	void SetAutoDelete(bool autoDelete); 

	bool started() const{return started_;}
	pid_t tid()const{return tid_;}
	const std::string& name()const{return name_;}
	
private:
	void setDefaultName();
	//纯虚函数 所以基类不能是实现 
	//RUN函数才是线程真正要运行的
	// void Run();
	//不能把这个线程调用函数声明为全局变量  所以放在类中私有化并声明为static
	//static void *ThreadRoutine(void *arg);
	bool started_;
	bool joined_; 
	pthread_t pthreadId_;
	pid_t tid_;
	
	//bool autoDelete_;
	ThreadFunc func_;
	std::string name_;
	CountDownLatch latch_;
};	
#endif
