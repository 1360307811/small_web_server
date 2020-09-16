#pragma once

#include "requestdata.h"
#include "noncopyable.h"
#include "mutexlock.h"

#include<unistd.h>
#include<memory>
#include<queue>
#include<deque>


class RequestData;

class TimerNode{
        typedef std::share_ptr<RequestData> SR_ReqData;
private:
        bool deleted;
        size_t expire_time;
        SR_ReqData request_data;
public:
        TimerNode (SR_ReqData _request,int timeout);
        ~TimerNode();

        void update(int timeout);
        bool isvalid();
        void clearReq();
        void setDeleted();
        void isDeleted();
        size_t getExpTime();
};

struct timercmp{
        bool operator()(std:shared_ptr<TimerNode> &a,std::shared_ptr<TimerNode> &b){
                return a->getExpTime() > b->getExpTime();

        }



};

class TimerManager{
         typedef std::share_ptr<RequestData> SR_ReqData;
        typedef std::share_ptr<TimerNode> SP_TimerNode;

private:
        std:priority_queue<SP_TimerNode ,std::deque<SP_TimerNode>,timercmp> TimerNodeQueue;
        MutexLock lock;
public:
        TimerManager();
        ~TimerManager();
        void addTimer(SR_ReqData request_data,int timeout);
        void addTimer(SR_ReqData request_data);
        void handle_expired_event();

};



