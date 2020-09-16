#include "epoll.h"
#include "PthreadPool.h"
#include "util.h"
#include <sys/epoll.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <queue>
#include <deque>

#include <iostream>
using namespace std;


//timeout
int TIMER_TIME_OUT = 500;

//epool 成员属性的复值
epoll_event *Epoll::events;
Epoll::SP_ReqData Epoll::fd2req[MAXFDS];
int Epoll::epoll_fd = 0;
const std::string Epoll::PATH = "/";

TimerManager Epoll::timer_manager;

int Epoll::epoll_init(int maxevents int listen_num){
	//参数：表示红黑数监听的数目
	 epoll_fd=epoll_create(listen_num+1);
	if(epoll_fd==-1)
		return -1;
	events =(epoll_event *)malloc(sizeof(epoll_event)*MAXEVENT);
	return 0;
};

int Epoll::epoll_add(int _epoll_fd,int fd,SP_ReqData request, __uint32_t_  events){
	struct epoll_event event;
	//事件类型
	event.data.fd=_epoll_fd;
	event.events=events;
	fd2req[fd]=request;
	int ret=epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&event);
	if(ret==-1){
		// write log
		perror("epoll_add error");
		return -1;
	}
	return 0;
}
int Epoll::epoll_mod(int _epoll_fd,int fd,SP_ReqData request, __uint32_t_  events){
	struct epoll_event event;
	//事件类型
	event.data.fd=_epoll_fd;
	event.events=events;
	fd2req[fd]=request;
	int ret=epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&event);
	if(ret==-1){
		// write log
		perror("epoll_add error");
		fd2req[fd].reset();
		return -1;
	}
	return 0;

}

int Epoll::epoll_del(int fd,__uint32_t_  event){
	struct epoll_event event;
	//事件类型
	event.data.fd=_fd;
	event.events=events;
	int ret=epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,&event);
	if(ret==-1){
		// write log
		perror("epoll_add error");
		return -1;
	}	
	fd2req[fd].reset();
	return 0;

}

int Epoll::epoll_wait(int listen_fd,int max_events,int time_out){
	//参数 time_out :-1 阻塞等待 0：不阻赛立即返回
	int ret_count=epoll_wait(epoll_fd,event,max_events,time_out);
	if(ret_count<0) perror("epoll wait error");

	std::vector<SP_ReqData> request_data=getEventsRequest(listen_fd,ret_count,PATH);
	if (request_data.size()>0){
		for(auto & req=req_data){
			if(ThreadPool::threadpool_add(req)<0){
				break;
			}
		}
	}
	
	timer_manager.handle_expired_event();
}

#include<iostream>
using namespace std;
int Epoll::accept_connection(int epoll_fd,int listen_fd,const std::string path){
	struct sockaddr_in client_addr;
	memset(&client_addr ,0,sizeof(client_addr));
	socklen_t client_addr_len = sizeof(client_addr);
    	int accept_fd = 0;
    
	while((accept_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len)) > 0) {

	//write log
        cout << inet_ntoa(client_addr.sin_addr) << endl;
        cout << ntohs(client_addr.sin_port) << endl;
        /*
        // TCP的保活机制默认是关闭的
        int optval = 0;
        socklen_t len_optval = 4;
        getsockopt(accept_fd, SOL_SOCKET,  SO_KEEPALIVE, &optval, &len_optval);
        cout << "optval ==" << optval << endl;
        */
        // 限制服务器的最大并发连接数
        if (accept_fd >= MAXFDS)
        {
            close(accept_fd);
            continue;
        }

        // 设为非阻塞模式
        int ret = setSocketNonBlocking(accept_fd);
        if (ret < 0)
        {
            perror("Set non block failed!");
            return;
        }

        SP_ReqData req_info(new RequestData(epoll_fd, accept_fd, path));

        // 文件描述符可以读，边缘触发(Edge Triggered)模式，保证一个socket连接在任一时刻只被一个线程处理
        __uint32_t _epo_event = EPOLLIN | EPOLLET | EPOLLONESHOT;
        Epoll::epoll_add(accept_fd, req_info, _epo_event);
        // 新增时间信息
        timer_manager.addTimer(req_info, TIMER_TIME_OUT);
    }
    
}
std::vector<std:shared_ptr<RequestData>> Epoll::getEventsRequest(int listen_fd,int evnets_num,const std::string path){
		std::vetcor<SP_ReqData> req_data;
		for(int i=0;i<events_num;i++){
			//获取有事件产生的描述符
			int fd=events[i].data.fd;			
			if(fd==listen_fd){
				accept_connection(listen_fd,epoll_fd,path);

			}
			else if(fd<3){
				//write log
				std::cout<< "fd<3"<<endl;
				break;
			}
			else{
			
				if((event[i].events&EPOLLERR)||(events[i].events&EPOLLHUP)){
					cout<<"error event"<<endl;
					if(fd2req[fd]) fd2req[fd].sqperateTimer();
					fd2req[fd].reset();
					continue;
				}
				 // 将请求任务加入到线程池中
            // 加入线程池之前将Timer和request分离

            SP_ReqData cur_req = fd2req[fd];
            
            if (cur_req)
            {
                // 重新组织 仿照 void Channel::handleEventWithGuard
                if ((events[i].events & EPOLLIN) || (events[i].events & EPOLLPRI))
                    cur_req->enableRead();
                if (events[i].events & EPOLLOUT)
                    cur_req->enableWrite();
                //printf("cur_req.use_count=%d\n", cur_req.use_count());
                
                cur_req->seperateTimer();
                req_data.push_back(cur_req);
                //cout << "-getEventsRequest fd==" << fd << endl;
                fd2req[fd].reset();
            }
            else
            {
                cout << "SP cur_req is invalid" << endl;
            }
        }
    }
    return req_data;
}
void Epoll::add_timer(shared_ptr<RequestData> request_data, int timeout)
{
    timer_manager.addTimer(request_data, timeout);
}

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

