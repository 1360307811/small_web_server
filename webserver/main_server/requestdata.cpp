#include "requestdata.h"
#include "util.h"
#include "epoll.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <queue>
#include <cstdlib>


#include<iostream>
using namespace std;
std::pthread_once_t MimeType::once_control=PTHREAD_ONCE_INIT;
std::unordered_map<std::string,std:string> MimeType::mine;

void MimeType::init(){
	 mime[".html"] = "text/html";
    mime[".avi"] = "video/x-msvideo";
    mime[".bmp"] = "image/bmp";
    mime[".c"] = "text/plain";
    mime[".doc"] = "application/msword";
    mime[".gif"] = "image/gif";
    mime[".gz"] = "application/x-gzip";
    mime[".htm"] = "text/html";
    mime[".ico"] = "application/x-ico";
    mime[".jpg"] = "image/jpeg";
    mime[".png"] = "image/png";
    mime[".txt"] = "text/plain";
    mime[".mp3"] = "audio/mp3";
    mime["default"] = "text/html";
}

std::string MimeType::getMime(const std::string str){
		pthread_once(&once_control,MimeType::init);//次函数只执行一次
		if(mime.find(str)==mime.end()){
			//没有匹配到类型 就以text/html文件格式发送
			return mime["default"];
		}else{
			return mime[str];
		}

}
	
RequestData::RequestData():
		now_read_pos=0;	
		state=STATE_PARSE_URI；
		h_satate=h_start;
		keepAlive=false;
		isAbleRead=true;
		isAbleWrite=false;
		events=0;
		error=false;
{

	cout<<"RequestDate construct"<<endl
}
 RequestData::RequestData(int _epollfd, int _fd, std::string _path)
		now_read_pos=0;	
		state=STATE_PARSE_URI；
		h_satate=h_start;
		keepAlive=false;
		isAbleRead=true;
		isAbleWrite=false;
		events=0;
		error=false;
		path=_path;
		fd=_fd;
		epoll_fd=_epollfd;
{
	cout<<"RequestDate construct"<<endl

}
    RequestData::~RequestData(){
	close(fd);
	cout<<"RequestDate destory"<<endl

}
    void RequestData::linkTimer(std::shared_ptr<TimerNode> mtimer){
	this->timer=mtimer;
}
    void RequestData::reset(){
		now_read_pos=0;	
		state=STATE_PARSE_URI；
		h_satate=h_start;
		
		inBuffer.clear();
		fileName.clear();
		path.clear();
		this->headers.clear();
	
		seperateTimer();
}
    void RequestData::seperateTimer(){
		if(timer.lock()){
			shared_ptr<TimerNode> my_timer=timer.lock();
			my_timer->clearReq();
			timer.reset();
		}


}
    int  RequestData::getFd();
    void RequestData::setFd(int _fd);
    void RequestData::handleRead(){
		do{
			int read_num=readn(fd,inBuffer);
			if(read_num<0){
				perror("readn error 1");
				error=true;
				handleError(fd,400,"Badquest");
				break;
			}
			else if(read_num==0){
				/*有请求连接 但没有数据 1：网络延迟 2：request abort 3:对端关闭 这种情况统一按3处理*/
				error=true;
				break;
			}

			if(state==STATE_PARSE_URI){
				int flag=this->parse_URI();
				if(flag==PARSE_URI_AGAIN){
					break;
				}
				else if(flag==PARSE_URI_ERROR){
					perror("parse_URI error 2");
					error=true;
					handleError(fd,400,"badquest");
					break;
				}
				else{
					/*flag==PARSE_URI_SUCCESS*/
					state=STATE_PARSE_HEADERS;
				}
			}
			else if(state==STATE_PARSE_HEADERS){
				int flag=this->parse_Headers();
				if(flag==PARSE_HEADER_AGAIN){
					break;
				}
				else if(flag==PARSE_HEADER_ERROR){
					perror("parse_URI error 3");
					error=true;
					handleError(fd,400,"badquest");
					break;
				}
				if(method==METHOD_POST){
					//如果头部是POST 那么数据还没有到达 所以要接受body
					state=STATE_RECV_BODY;
				}else{
					state=STATE_ANALYSIS;
				}
			}
			else if(state==STATE_RECV_BODY){


			}
			else if(state==STATE_ANALYSIS){

				
			}

		
		}while(false);


}
    void RequestData::handleWrite();
    void RequestData::handleError(int fd, int err_num, std::string short_msg);
    void RequestData::handleConn();

    void RequestData::disableReadAndWrite();

    void RequestData::enableRead();

    void RequestData::enableWrite();

    bool RequestData::canRead();

    bool RequestData::canWrite();
/* 获取请求行（请求方法,URL,HTTP版本号） 从inbuffer中读取 读取完后从 inbuffer 清除请求行部分 然后将解析的数据 复制给this 的   
 * 		 int method;
 *     		 int HTTPversion;
 *               std::string file_name;   这3个成员*/

 int parse_URI();
/*解析请求首部子段 然后将首部的key value存放在std::unordered_map<std::string, std::string> headers;成员中
 * 解析方式 ：首先在读取inbuffer中的首部子段 利用enum HeadersState 循环访问inbuffer中的数据
 *最后 ：清除inbuffer中在parse_Headers中分析好的部分
 *
 * */
    int parse_Headers();
/* 分析请求 首先看请求 是POST GET */
    int analysisRequest();
