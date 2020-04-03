#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>
#include"threadpool.h"

void *process(void *arg){
     printf("thread %lx now in process function\n", pthread_self());
     int *a = (int *)arg;
     printf("arg = %d\n", *a);

     sleep(2);      //模拟处理任务


     return NULL;
 }

int main()
{

//threadpool_t *threadpoll_create(int min_thr_num, int max_thr_num, int queue_max_size)
    threadpool_t *tpool = threadpoll_create(10, 100,  100);   //
    printf("pool init\n");


    int num[6] = {1, 2, 3, 4 ,5, 6};
    for(int  j = 0; j <100; j++){
    for(int i = 0; i < 6; i++)
        threadpool_add(tpool, process, (void *)&num[i]);
    }

    sleep(80);                       //模拟子线程完成任务
    printf("anothrer \n");
    for(int  j = 0; j <100; j++){
    for(int i = 0; i < 6; i++)
        threadpool_add(tpool, process, (void *)&num[i]);
    }

    threadpool_destroy(tpool);
    printf("destroy all\n");
    sleep(1000);
    




    return 0;
}
