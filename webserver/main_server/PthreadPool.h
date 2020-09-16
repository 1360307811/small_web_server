#ifndef __PTHREADPOOL_H__
#define __PTHREADPOOL_H__

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<stdint.h>
typedef struct tk_task{
	void (*func)(void*);
	void* arg;
	struct tk_task* next;
}tk_task_t;

typedef struct threadpool{
	pthread_mutex_t lock;
	pthread_cond_t cond;
	pthread_t *threads;
	tk_task_t *head;
	int thread_count;
	int queue_size;
	int shutdown;
	int started;
}tk_threadpool_t;

typedef enum{
	/*初始化失败*/
	tk_tp_invalid= -1,
	/*上锁失败*/
	tk_tp_lock_fail= -2,
	/*线程池已经关闭*/
	tk_tp_already_shutdown= -3,
	/*线程*/
	tk_tp_cond_broadcast= -4,
	/*线程问题*/
	tk_tp_thread_fail= -5
}tk_threadpool_error_t;

typedef enum{
	//立即关掉
	immediate_shutdown= 1,
	//超时关掉
	graceful_shutdown= 2
}tk_threadpool_sd_t;

tk_threadpool_t* threadpool_init(int thread_num);
int threadpool_add(tk_threadpool_t* poll,void(*func)(void *),void* arg);
int threadpool_destroy(tk_threadpool_t* pool,int graceful);

#endif

