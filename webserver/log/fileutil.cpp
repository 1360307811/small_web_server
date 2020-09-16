#include"fileutil.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

AppendFile::AppendFile(string fileName){
	fp_=fopen(fileName.c_str(),"ae");
	/*?????*/
	setbuffer(fp_,buffer_,sizeof(buffer_));
}

AppendFile::~AppendFile(){

	fclose(fp_);
	//delete buffer_;
}

void AppendFile::append(const char* logline,const size_t len){
	size_t 	n=0,residue=len;
	while(residue>0){
		n=write(logline,residue);
		if(n==-1){
			int err=ferror(fp_);
			if(err){	
				fprintf(stderr,"AppendFile ::append() failed!\n");
			}
			break;
		} 
		residue=residue-n;
	}
}


void AppendFile::flush(){
	fflush(fp_);/*因为给fp_设置了缓冲区  所以的刷新buffer_*/
}

size_t AppendFile::write(const char * logline,size_t len){
	return fwrite_unlocked(logline,1,len,fp_);
}

/*
 ae:Open for reading and appending (writing at end of file).  The file is created if it does not exis 

setbuffer:在打开文件流后，读取内容之前，调用setbuffer()可用来设置文件流的缓冲区。参数stream为指定的文件流，参数buf指向自定的缓冲区起始地址，参数size为缓冲区大小。
 

fwrite_unlocked:只是它们不使用锁定（它们不要设置锁本身，也不要测试是否存在由其他人设置的锁），因此是线程不安全的。参见flockfile（3）。
 

ferror:在调用各种输入输出函 数(如 putc.getc.fread.fwrite等)时，如果出现错误，除了函数返回值有所反映外，还可以用ferror函数检查。 它的一般调用形式为 ferroe(fp)；如果ferror返回值为0(假)，表示未出错。如果返回一个非零值，表示出错。应该注意，对同一个文件 每一次调用输入输出函数，均产生一个新的ferror函 数值，因此，应当在调用一个输入输出函数后立即检 查ferror函数的值，否则信息会丢失。在执行fopen函数时,ferror函数的初始值自动置为0。
 * */



