#pragma once 
#include "RequestData.h"
#include "Timer.h"
#include<sys/epoll.h>
#include<vector>
#include<memory>
#include<unordered_map>
#include<unsitd.h>




class channel;
//class loopInfo;
 
class eventloop;

class Epoll{
public:
	typedef std::shared_ptr<RequestData> SR_ReqData;

public:
	
	static int epoll_init(int max_event,int listnum);
	static int epoll_add(int fd,SR_ReqData request, __uint32_t events);
	static int epoll_mod(int fd,SR_ReqData request, __uint32_t events);
	static int epoll_del(int fd, __uint32_t events=(EPOLLIN | EPOLLET| EPOLLONESHOT));
	static void epoll_wait(int listen_fd ,int max_events,int timeout);
	static AcceptConnection(int listen_fd,int epoll_fd,const std::string path );
	static std::vector<SR_ReqData> getEventsRequest(int listen_fd,int events_num,const std::string path);

	static void add_timer(SR_ReqData request_data ,int timeout);
	
private:
	static const int MAXFDS=1000;
	static epoll_event *events;
	static SR_ReqData fd2req[MAXFDS];
	static int epoll_fd;
	static const std::string PATH;

	static TimerManager time_manager; 
};
