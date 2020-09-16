#include"logfile.h"
#include"fileutil.h"
#include<time.h>
#include<assert.h>
#include<stdio.h>
using namespace std;
LogFile::LogFile(const string& basename,int flushcount):
        basename_(basename),flush_(flushcount),mutex_(new MutexLock){
        count_=0;
        file_.reset(new AppendFile(basename));
}


LogFile::~LogFile(){

}


void LogFile::append(const char* logline,int len){
        MutexLockGuard lock(*mutex_);
        append_unlocked(logline,len);
}



void LogFile::flush(){
        MutexLockGuard lock(*mutex_);
        file_->flush();
}
 void LogFile::append_unlocked(const char* logline,int len){
        file_->append(logline,len);
        ++count_;
        if(count_>=flush_){
                count_=0;
                file_->flush();
        }

}

int main(){
        LogFile logfile("test.txt",3);
        logfile.append("111111111111111111111111111111\n",20);
        logfile.append("111111111111111111111111111111\n",20);
        logfile.append("111111111111111111111111111111\n",20);
        logfile.append("111111111111111111111111111111\n",20);

}
               


                                                                                                       

