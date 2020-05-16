#include<iostream>
#include<string>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>
#include <sys/wait.h>
#include<pwd.h>
#include<vector>
#include<string.h>
 #include <sys/types.h>
 #include <sys/stat.h>
#include <fcntl.h>

using namespace std;

class shell{
public:
	int shell_run(){
	string str;
		int p[2];
					pipe(p);
	 while(1)
    	{
		if(str.empty()){
      		GetLoginName();
       		GetHostName();
        	GetDir();
        	fflush(stdout);
		getline(cin,str);
		}
			//获取字符 并分解
		vector<string> vec;
		int sfd, pipe_sfd=-1,testfd;
		int flag=-1,pipe_flag=-1;
       		for(int i=0,k=0;i<str.size();i++){
			
			if(str[i]==' '){
				string temp=str.substr(k,i-k);
				vec.push_back(temp);
				k=i+1;
			}else if(i==str.size()-1){
				string temp=str.substr(k,i-k+1);
				vec.push_back(temp);
			}
			else if(str[i]=='>' ){
				flag=1;
				if(str[i+1]=='>'){
					//追加重定向
				}else{
					//输出重定向
					sfd = dup(STDOUT_FILENO);
					int l=i+1;
					while(str[l]!=' '){
						if(l==(str.size()-1)){
							l++;
							break;
						}
					  l++;
					}
					string temp=str.substr(i+1,l-i-1);
					testfd = open(temp.c_str(),O_CREAT | O_RDWR );
      					  if (-1 == testfd)
   					     {
           					     printf("open file error.\n");
             					   exit(1);
      					  }

    						    /* 重定向 */
      					  if (-1 == dup2(testfd,STDOUT_FILENO) ) {
              				  printf("can't redirect fd error\n");
              				  exit(1);
     					   }
					
					 i=l+1;
					k=l+1;
					continue;
				
				}			
			}

			else if(str[i]=='<'){
					flag=0;
					sfd = dup(STDIN_FILENO);
					int l=i+1;
					while(str[l]!=' '){
						if(l==(str.size()-1)){
							l++;
							break;
						}
					  l++;
					}
					string temp=str.substr(i+1,l-i-1);
					testfd = open(temp.c_str(), O_RDWR);
      					  if (-1 == testfd)
   					     {
           					     printf("open file error.\n");
             					   exit(1);
      					  }

    						    // 重定向 
      					  if (-1 == dup2(testfd,STDIN_FILENO) ) {
              				  printf("can't redirect fd error\n");
              				  exit(1);
     					   }
					
				 	i=l+1;
					k=l+1;
					continue;
			
	

			}
			else if(str[i]=='2'&& str[i+1]=='>'){
				flag=2;
				sfd = dup(STDERR_FILENO);
					int l=i+1;
					while(str[l]!=' '){
						if(l==(str.size()-1)){
							l++;
							break;
						}
					  l++;
					}
					string temp=str.substr(i+1,l-i-1);
					testfd = open(temp.c_str(), O_CREAT| O_RDWR);
      					  if (-1 == testfd)
   					     {
           					     printf("open file error.\n");
             					   exit(1);
      					  }

    						    // 重定向 
      					  if (-1 == dup2(testfd,STDERR_FILENO) ) {
              				  printf("can't redirect fd error\n");
              				  exit(1);
     					   }
					
				 	i=l+1;
					k=l+1;
					continue;
					

			}else if(str[i]=='|'){
					pipe_flag=1;
					/* if (-1 == dup2(p[1],STDIN_FILENO) ) {
              				  printf("can't redirect fd error\n");
              				  exit(1);
     					   }
					*/
						pipe_sfd = dup(STDOUT_FILENO);

					 if (-1 == dup2(p[0],STDOUT_FILENO) ) {
              				  printf("can't redirect fd error\n");
              				  exit(1);
     					   }
					if(str[i+1]==' ') i=i+1;

					str=str.substr(i+1,str.size()-i-1);
					break;

			}
			
		}
		char* MyArgv[10];
        	for(int m = 0; m <vec.size();++m)
        	{
            		//printf("第%d参数是%s\n",m,vec[m].c_str());
			MyArgv[m]=new char[vec[m].size()];
			strcpy(MyArgv[m],vec[m].c_str());
        	}
	
		MyArgv[vec.size()]=NULL;//如果这一步不加的话就会出现错误 
                //fork新的进程
       		 int  id = fork();
        	if(id == 0)
        	{
			
			//cout<<"执行我的操作："<<MyArgv[0]<<MyArgv[1]<<endl;
            		//child,执行替换操作
            		execvp(MyArgv[0],MyArgv);
           		perror("error");
            		exit(1);
        	}
        	else
        	{
            	
            	
			wait(NULL);
       			/* 恢复stdout */
			//cout<<sfd<<endl;
			if(flag==1){
       			 if (-1 == dup2(sfd,STDOUT_FILENO) ) {
			
				perror("dup2 error");
				exit(1);               		
	        		}
			}else if(flag==0){
				 if (-1 == dup2(sfd,STDIN_FILENO) ) {
			
				perror("dup2 error");
				exit(1);               		
	        		}

			}else if(flag==2){
				 if (-1 == dup2(sfd,STDERR_FILENO) ) {
			
				perror("dup2 error");
				exit(1);               		
	        		}


			}else{}
			if(pipe_flag==1){
				 if (-1 == dup2(pipe_sfd,STDOUT_FILENO) ) {
			
				perror("dup2 error");
				exit(1);               		
	        		}
				pipe_sfd = dup(STDIN_FILENO);

					 if (-1 == dup2(p[1],STDIN_FILENO) ) {
              				  printf("can't redirect fd error\n");
              				  exit(1);
     					   }

				pipe_flag=2;

			
			}else if(pipe_flag==2){
				if (-1 == dup2(pipe_sfd,STDIN_FILENO) ) {
			
				perror("dup2 error");
				exit(1);               		
	        		}
				pipe_flag=-1;
				
			}else{}
			close(testfd);
		}
	    }

	//close(p[0]);
	//close(p[1]);
    return 0;

}


private:
	void GetLoginName()
	{
    		struct passwd* pass;
    		pass = getpwuid(getuid());//getwuid 根据getuid获得的运行进行的用户id 获取当前的用户信息
    		printf("[%s@",pass->pw_name);
	}
	void GetHostName()
	{	
    		char name[128];
    		gethostname(name,sizeof(name)-1);//访问主机名
    		printf("%s",name);
	}
	void GetDir()
	{
    		char pwd[128];
    		getcwd(pwd,sizeof(pwd)-1);//测试结果:/home/LInux编程技术

    		int len = strlen(pwd);
	
   		 char* p = pwd+len-1;//将指针指向该字符串的最后一个字母
    		while(*p != '/' && len--)
    		{
       			 p--;
   		 }
    		p++;

    		printf(" %s]我的shell模式 : ",p);
	}

};
int main(){
	class shell s;
	s.shell_run();
}


