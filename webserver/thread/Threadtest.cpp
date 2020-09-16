#include"Thread.h"
//#include<boost/bind.hpp>
#include<iostream>
#include<unistd.h>
#include<pthread.h>
using namespace std;


void ThreadFunc(){
	cout<<"ThreadFunc...."<<endl;
}
int main(){
	string name="log";
	Thread t1(ThreadFunc,name);
	t1.start();
	t1.join();
	return 0;
}

