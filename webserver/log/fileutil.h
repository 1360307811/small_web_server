#pragma once
#include"../noncopyable.h"
//#include<csting>
#include<string>
class AppendFile:noncopyable{
public:
	/*显示构造*/
	explicit AppendFile(std::string filename);
	~AppendFile();
	/*向缓冲区增加日志*/
	void append(const char* logline,const size_t len);
	/*刷新日志*/
	void flush();
private:
	/*真正写日志*/
	size_t write(const char* logline,size_t len);
	FILE * fp_;
	char buffer_[64*1024];

};
