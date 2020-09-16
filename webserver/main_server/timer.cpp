#include"timer.h"
#include"epoll.h"

#include<unordered_map>
#include<string>
#include<sys/time.h>
#include<unistd.h>
#include<deque>
#include<queue>

#include<iostream>

using namespace std;


TimeNode::TimerNode (SR_ReqData _request,int timeout){
	deleted=false;
	request_data=_request;
	struct timeval now;
	gettimeofday(&now);
	//以毫米为单位
	expire_time=((now.tv_sec*1000)+(now.tv_usec/1000))+timeout;
}
TimerNode::~TimerNode(){
	if(request_data){
		Epoll::epoll_del(request_data->getFd());
	}
	//因为这是虚构函数 自己释放整个对象
	
}

void TimerNode::update(int timeout){
	struct timeval now;
	gettimeofday(&now);
	//以毫米为单位
	expire_time=((now.tv_sec*1000)+(now.tv_usec/1000))+timeout;


}
//判断当前时间节点是否可用
bool TimerNode::isvalid(){
	struct timeval now;
	gettimeofday(&now);
	//以毫米为单位
	size_t temp=((now.tv_sec*1000)+(now.tv_usec/1000))+timeout;
	if(temp<expire_time){
		return true;
	}else{
		//将这个节点设置为不可用
		this->setDeleted();
		return false;
	}

}
//清除当前时间节点指向的request_data
 void TimerNode::clearReq(){
	request_data.reset();
	this->setDeleted;
}

 void TimerNode::setDeleted(){
	deleted=true;
}
 void TimerNode::isDeleted(){
	return deleted;
}
size_t TimerNode::getExpTime(){
	return expire_time;
}:

TimerManager::TimerManager()
{
}

TimerManager::~TimerManager()
{
}

void TimerManager::addTimer(SP_ReqData request_data, int timeout)
{
    SP_TimerNode new_node(new TimerNode(request_data, timeout));
    {
        MutexLockGuard locker(lock);
        TimerNodeQueue.push(new_node);
    }
    request_data->linkTimer(new_node);
}

void TimerManager::addTimer(SP_TimerNode timer_node)
{

}
void TimerManager::handle_expired_event()
{
    MutexLockGuard locker(lock);
    while (!TimerNodeQueue.empty())
    {
        SP_TimerNode ptimer_now = TimerNodeQueue.top();
        if (ptimer_now->isDeleted())
        {
            TimerNodeQueue.pop();
            //delete ptimer_now;
        }
        else if (ptimer_now->isvalid() == false)
        {
             TimerNodeQueue.pop();
             //delete ptimer_now;
        }
        else
         {
            break;
  	 }
      }
  }

