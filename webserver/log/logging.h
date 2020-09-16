

#pragma once 
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "logstream.h"
class Logger{
public:
	 Logger(const char *filename,int line);
	 ~Logger()/*当希构Logger时  就会把Logger写进Asynclogging中*/;
	 LogStream& stream(){return impl.stream_};
	
private:

	class Impl{
	public:
		Impl(const char* filename,int line)
			: stream_(),line_(line),basename_(fileName){
    			formatTime();
		}

		
		void formatTime(){
			struct timeval tv;
    			time_t time;
    			char str_t[26] = {0};
   			 gettimeofday (&tv, NULL);
    			time = tv.tv_sec;
    			struct tm* p_time = localtime(&time);   
    			strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
   			 stream_ << str_t;

		}

		LogStream stream_;
		int line_;
		std::string basename_; 

	};
	Impl impl;
	static std::string logFimeName_;

};


#define LOG Logger(__FILE__,__LINE__).stream();
