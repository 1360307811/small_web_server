#pragma once
#include"timer.h"
#include<string>
#include<memory>
#include<sys/epoll.h>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;
//此时读取局柄fd数据 的状态  根据状态读取  不用一次全部读取 
const int STATE_PARSE_URI = 1;
const int STATE_PARSE_HEADERS = 2;
const int STATE_RECV_BODY = 3;
const int STATE_ANALYSIS = 4;
const int STATE_FINISH = 5;

//缓冲区大小
const int MAX_BUFF = 4096;

const int AGAIN_MAX_TIMES = 200;

//解析URI的结果
 const int PARSE_URI_AGAIN = -1;
 const int PARSE_URI_ERROR = -2;
 const int PARSE_URI_SUCCESS = 0;

//解析header的结果
 const int PARSE_HEADER_AGAIN = -1;
 const int PARSE_HEADER_ERROR = -2;
 const int PARSE_HEADER_SUCCESS = 0;

 const int ANALYSIS_ERROR = -2;
 const int ANALYSIS_SUCCESS = 0;

 const int METHOD_POST = 1;
 const int METHOD_GET = 2;
 const int HTTP_10 = 1;
 const int HTTP_11 = 2;

const int EPOLL_WAIT_TIME=500;

//获取资源类
class MimeType{
private:
	static void init();
	static std::unordered_map<std::string,std:string> mine;
	MimeType();
	MimeType(const MimeType &m);
public:
	static std::string getMime(const std::string &str);
private :
	static pthread_once_t once_control;
	/*一次初始化-pthread_once_t pthread_once 函数*/
}；

//解析头部每行的位置
enum HeaderState{
	h_start=0,/*开始*/
	h_key,/*首部某行key的位置*/
	h_colon,/*首部某行冒号的位置*/
	h_spaces_after_colon,/*首部某行冒号后面的位置*/
    h_value,/*首部某行值的位置*/
    h_CR,/*首部某行\r的位置*/
    h_LF,/*首部某行\n的位置*/
    h_end_CR,/*首部最后一行\r位置*/
    h_end_LF/*首部最后一行\n位置*/
};


class TimerNode;


class RequestData:public:std::enable_shared_from_this{
private:
	
	std::string path;//工作目录
	int fd;		//次请求数据对应的fd
	inr epoll_fd;
	__uint32_t events;//事件
	
	//缓冲区
	std::string inBuffer;
	std::string outBuffer;

	bool error;
	
	int method;//方式get/post
	int httpVersion;//http版本
	std::string fileName;//url
	int now_read_pos;
	int state;//此时数据处理的状态
	int h_state;//头
	bool isFinish;
	bool keepAlive;
	std::unordered_map<std::string,std::string> headers;//
	std::weak_ptr<TimerNode> timer;//这个数据处理的时间
	
	bool isAbleRead;
	bool isAbleWrite;

private:
	int parse_URI();//提取解析URI
	int parse_Headers();//解析头部
	int analyisRequest();//分析请求

	Mat stitch(Mat & src){
		return src;
	}

public:
	
RequestData();
    RequestData(int _epollfd, int _fd, std::string _path);
    ~RequestData();
    void linkTimer(std::shared_ptr<TimerNode> mtimer);//使Request 和timer连接起来
    void reset();//重置
    void seperateTimer();//清楚事件节点中的requet 并将时间节点的delete设置为true 表示时间节点不可用
    int getFd();	
    void setFd(int _fd);
    void handleRead(); //局柄（文件描述符）读
    void handleWrite();//局柄写
    void handleError(int fd, int err_num, std::string short_msg);//局柄错误处理
    void handleConn();//局柄连接

    void disableReadAndWrite();

    void enableRead();

    void enableWrite();

    bool canRead();

    bool canWrite();
	i
};
