#pragma once

#include<assert.h>
#include<string.h>
#include<string>
#include"noncopyable.h"
class AsyncLogging;
const int kSmallBuffer=4000;
const int kLargeBuffer=4000*1000;


template<int SIZE>
/*一个固定大小的缓冲区 是写进log 文件的中间缓冲区*/
class FixedBuffer:noncopyable{
public:
	FixedBuffer();
	~FixedBuffer();
	void append(const char* buf,size_t len){
		if(avalid()> len){
			memcpy(cur_,buf,len);
			cur+=len;
		}

	}
	const char * data(){
		return data_;
	}
	
	int length(){
		static_cast<int>(cur_-data);
	}
	
	char *current(){
		return cur_;
	}
	
	int avalid()const{
		static_cast<int(end()-cur_);
	}
	
	void add(size_t len){
		cur_+=len;	
	}
	
	void reset(){
		cur_=data_;
	}
	
	void bzero(){
		memset(data_,0,sizeof(data_));

	}
privateL:
	const char* end() const{
		return data_+sizeof(data_);
	}
	
	/*缓冲区*/
	char data_[SIZE];
	/*缓冲区中有数据部分和无数据部分的分界出*/
	char * cur_;
};

class LogStream:noncopyable{

typedef LogStream self;
public:
	typedef FixedBuffer<kSmallBuffer> Buffer；
	self& operator<<(bool v){
		buffer_.append(v?"1":"0",1);
		return *this;
	}

	  LogStream& operator<<(short);
  LogStream& operator<<(unsigned short);
  LogStream& operator<<(int);
  LogStream& operator<<(unsigned int);
  LogStream& operator<<(long);
  LogStream& operator<<(unsigned long);
  LogStream& operator<<(long long);
  LogStream& operator<<(unsigned long long);

  LogStream& operator<<(const void*);

  LogStream& operator<<(float v) {
    *this << static_cast<double>(v);
    return *this;
  }
  LogStream& operator<<(double);
  LogStream& operator<<(long double);

  LogStream& operator<<(char v) {
    buffer_.append(&v, 1);
    return *this;
  }

  LogStream& operator<<(const char* str) {
    if (str)
      buffer_.append(str, strlen(str));
    else
      buffer_.append("(null)", 6);
    return *this;
  }

  LogStream& operator<<(const unsigned char* str) {
    return operator<<(reinterpret_cast<const char*>(str));
  }

  LogStream& operator<<(const std::string& v) {
    buffer_.append(v.c_str(), v.size());
    return *this;
  }

	void append(const char * data,int len){
		buffer_.append(data,len);
	}
	
	void Buffer& buffer()const{
		return buffer_;
	}
	
	void resetBuffer(){
		buffer_.reset();
	}
private:
	void staticCheck();
	template<typename T>
		void formatInteger(T);/*转换个时*/
	

	Buffer buffer_;

	static const int kMaxNumericSize=32;
};
