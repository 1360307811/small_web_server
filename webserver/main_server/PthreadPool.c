
#include "PthreadPool.h"
static int thread_free(tk_threadpool_t* pool);
static void* threadpool_worker(void* arg);
//释放线程池
int thread_free(tk_threadpool_t* pool){
	if(pool==NULL||pool->started>0){
		return -1;
	}
	if(pool->threads){
		free(pool->threads);
	}

	//逐点销毁tk_task链表
	tk_task_t *old;
	while(pool->head->next){
		old=pool->head->next;
		pool->head->next=old->next;
		free(old);
	}
	return 0;
}

void *thread_worker(void* arg){
	if(arg==NULL){
		return NULL;
	}
	//printf("开始工作\n");
	tk_threadpool_t *pool=(tk_threadpool_t*)arg;
	//	printf("pool->shutdown:%d\n",pool->shutdown);

	tk_task_t* task;
		while(1){
			//对线程池上锁
			pthread_mutex_lock(&(pool->lock));
	
			//没有task且未停机且阻塞
			while((pool->queue_size==0)&& !(pool->shutdown))
				pthread_cond_wait(&(pool->cond),&(pool->lock));
			

			if(pool->shutdown==immediate_shutdown){
			//	printf("pool->shutdown:%d\n",pool->shutdown);
				break;
			}
			else if((pool->shutdown==graceful_shutdown)&& (pool->queue_size==0))
				break;
			//得到第一个task
			task=pool->head->next;			
			
			//没有task则开锁并进行下一个循环
			if(task==NULL){
				pthread_mutex_unlock(&(pool->lock));
				continue;
			}
			
			//存在task 则取走并开锁
			pool->head->next=task->next;
			pool->queue_size--;
			pthread_mutex_unlock(&(pool->lock));

		//	printf("执行任务\n");
			//task->func(task->task);
			(* (task->func))(task->arg);
			free(task);
		}

	pool->started--;
	pthread_mutex_unlock(&(pool->lock));
	pthread_exit(NULL);
	return NULL;
}
int threadpool_destroy(tk_threadpool_t* pool,int graceful){
	if(pool==NULL){
		return tk_tp_invalid;
	}
	if(pthread_mutex_lock(&(pool->lock))!=0){
		return tk_tp_lock_fail;

	}
	int err=0;
	do{
		if(pool->shutdown){
			err=tk_tp_already_shutdown;
			break;
		}
		pool->shutdown=(graceful)?graceful_shutdown:immediate_shutdown;	
		if(pthread_cond_broadcast(&(pool->cond))){
			err=tk_tp_cond_broadcast;
			break;
		}

		if(pthread_mutex_unlock(&(pool->lock))!=0){
			err=tk_tp_lock_fail;
			break;
		}
		int i;
		for(i=0;i<pool->thread_count;i++){
			if(pthread_join(pool->threads[i],NULL)!=0){
				err=tk_tp_thread_fail;
			}
		}
	}while(0);
	
	if(!err){
		//pthread_mutex_destroy(pthread_mutex_t *mutex);
		pthread_mutex_destroy(&(pool->lock));
		pthread_cond_destroy(&(pool->cond));
		thread_free(pool);
	}
	return err;
}

tk_threadpool_t *threadpool_init(int thread_num){
	tk_threadpool_t* pool;
	pool=(tk_threadpool_t* )malloc(sizeof(tk_threadpool_t));
	if(pool==NULL){
		goto err;

	}

	//初始化；
	pool->thread_count=0;
	pool->queue_size=0;
	pool->shutdown=0;
	pool->started=0;
	pool->threads=(pthread_t *)malloc(sizeof(pthread_t)*thread_num);

	//初始化任务链表
	pool->head=(tk_task_t*)malloc(sizeof(tk_task_t));
	if((pool->threads==NULL)&&(pool->head==NULL)){
		goto err;
	}
	pool->head->func=NULL;
	pool->head->arg=NULL;
	pool->head->next=NULL;
	
	//初始化锁
	if(pthread_mutex_init(&(pool->lock),NULL)!=0){
		goto err;
	}
	if(pthread_cond_init(&(pool->cond),NULL)!=0){
		goto err;
	}
	int i;
	printf("初始化完成 开始创建线程");
	for(i=0;i<thread_num;i++){
		if((pthread_create(&(pool->threads[i]),NULL,thread_worker,(void*)pool)!=0)){
			threadpool_destroy(pool,0);
			return 0;
		}
		pool->thread_count++;
		pool->started++;
	}
	return pool;
err:
	if(pool){
		thread_free(pool);
	}
	return NULL;
}


int threadpool_add(tk_threadpool_t* pool,void (*func)(void *),void* arg){
		int rc,err=0;
		if(pool==NULL,func==NULL){
			return -1;
		}
		if(pthread_mutex_lock(&pool->lock)!=0){
			return -1;
		}

		//判度线程是否被销毁
		if(pool->shutdown){
			err=tk_tp_already_shutdown;
			goto out;
		}
		
		//创建任务节点 然后把他加入到任务链表种
		tk_task_t* task=(tk_task_t* )malloc(sizeof(tk_task_t));
		if(task==NULL){
			goto out;
		}	
		task->func=func;
		task->arg=arg;
		task->next=pool->head->next;
		pool->head->next=task;
	
		pool->queue_size++;
		rc=pthread_cond_signal(&(pool->cond));
out:

		if(pthread_mutex_unlock(&(pool->lock))!=0){
			return -1;
		}else{
			return err;
		}
}

/*
void test(void* arg){
	int *i=(int *)arg;
	printf("工作%d开始处理\n",*i);
}
int main(){
	tk_threadpool_t* pool=threadpool_init(6);
	int i=0;
	for(i=0;i<6;i++){
		int k=threadpool_add(pool,test,(void*)&i);
	//	printf("%d任务已加\n",i);
	//	printf("%d\n",k);
	}

	sleep(10);
	threadpool_destroy(pool,0);
}*/
